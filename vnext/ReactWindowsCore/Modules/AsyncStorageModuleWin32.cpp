#include "pch.h"

#include "AsyncStorageModuleWin32.h"
#include "AsyncStorageModuleWin32Config.h"

#include <cstdio>

/// Implements AsyncStorageModule using winsqlite3.dll (requires Windows version 10.0.10586)

namespace {

std::string &AsyncStorageDBPath() {
  static std::string asyncStorageDBPath;
  return asyncStorageDBPath;
}

void InvokeError(facebook::xplat::module::CxxModule::Callback &callback, const char *message) {
  callback({folly::dynamic::object("message", message)});
}

void ExecImplThrows(
    sqlite3 *db,
    const char *statement,
    facebook::react::AsyncStorageModuleWin32::ExecCallback execCallback = nullptr,
    void *pv = nullptr) {
  char *errMsg = nullptr;
  int rc = sqlite3_exec(db, statement, execCallback, pv, &errMsg);
  if (errMsg) {
    std::runtime_error exception(errMsg);
    sqlite3_free(errMsg);
    sqlite3_close(db);
    throw exception;
  }
  if (rc != SQLITE_OK) {
    std::runtime_error exception(sqlite3_errmsg(db));
    sqlite3_free(errMsg);
    sqlite3_close(db);
    throw exception;
  }
}

bool ExecImpl(
    sqlite3 *db,
    const char *statement,
    facebook::react::AsyncStorageModuleWin32::ExecCallback execCallback,
    void *pv,
    facebook::xplat::module::CxxModule::Callback &callback) {
  char *errMsg = nullptr;
  int rc = sqlite3_exec(db, statement, execCallback, pv, &errMsg);
  if (errMsg) {
    InvokeError(callback, errMsg);
    sqlite3_free(errMsg);
    return false;
  }
  if (rc != SQLITE_OK) {
    InvokeError(callback, sqlite3_errmsg(db));
    return false;
  }
  return true;
}

bool CheckSize(sqlite3 *db, folly::dynamic &args, facebook::xplat::module::CxxModule::Callback &callback) {
  if (!args.isArray()) {
    InvokeError(callback, "Invalid keys type. Expected an array");
    return false;
  }
  int varLimit = sqlite3_limit(db, SQLITE_LIMIT_VARIABLE_NUMBER, -1);
  auto argCount = args.size();
  if (argCount > INT_MAX || static_cast<int>(argCount) > varLimit) {
    char errorMsg[60];
    sprintf_s(errorMsg, "Too many keys. Maximum supported keys :%d", varLimit);
    InvokeError(callback, errorMsg);
    return false;
  }
  return true;
}

class Sqlite3Transaction {
  sqlite3 *m_db{nullptr};
  facebook::xplat::module::CxxModule::Callback *m_callback{nullptr};

 public:
  Sqlite3Transaction() = default;
  Sqlite3Transaction(sqlite3 *db, facebook::xplat::module::CxxModule::Callback &callback)
      : m_db(db), m_callback(&callback) {
    if (!ExecImpl(m_db, u8"BEGIN TRANSACTION", nullptr, nullptr, *m_callback)) {
      m_db = nullptr;
      m_callback = nullptr;
    }
  }
  Sqlite3Transaction(const Sqlite3Transaction &) = delete;
  Sqlite3Transaction(Sqlite3Transaction &&other) : m_db(other.m_db), m_callback(other.m_callback) {
    other.m_db = nullptr;
    other.m_callback = nullptr;
  }
  Sqlite3Transaction &operator=(const Sqlite3Transaction &) = delete;
  Sqlite3Transaction &operator=(Sqlite3Transaction &&rhs) {
    if (this != &rhs) {
      Commit();
      std::swap(m_db, rhs.m_db);
      std::swap(m_callback, rhs.m_callback);
    }
  }

  explicit operator bool() const {
    return m_db != nullptr;
  }

  void Rollback() {
    if (m_db) {
      ExecImpl(m_db, u8"ROLLBACK", nullptr, nullptr, *m_callback);
      m_db = nullptr;
      m_callback = nullptr;
    }
  }

  bool Commit() {
    if (!m_db) {
      return false;
    }
    auto result = ExecImpl(m_db, u8"COMMIT", nullptr, nullptr, *m_callback);
    m_db = nullptr;
    m_callback = nullptr;
    return result;
  }

  ~Sqlite3Transaction() {
    Rollback();
  }
};

std::string MakeSQLiteParameterizedStatement(const char *prefix, int argCount) {
  assert(argCount != 0);
  std::string result(prefix);
  result.reserve(result.size() + (argCount * 2) + 1);
  result += '(';
  for (int x = 0; x < argCount - 1; x++) {
    result += "?,";
  }
  result += "?)";
  return result;
}

} // namespace

namespace facebook {
namespace react {

AsyncStorageModuleWin32::AsyncStorageModuleWin32() {
  if (sqlite3_open_v2(
          AsyncStorageDBPath().c_str(),
          &m_db,
          SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX,
          nullptr) != SQLITE_OK) {
    auto exception = std::runtime_error(sqlite3_errmsg(m_db));
    sqlite3_close(m_db);
    throw exception;
  }

  int userVersion = 0;
  auto getUserVersionCallback = [](void *pv, int cCol, char **rgszColText, char **rgszColName) {
    if (cCol < 1) {
      return 1;
    }
    *static_cast<int *>(pv) = atoi(rgszColText[0]);
    return SQLITE_OK;
  };

  ExecImplThrows(m_db, u8"PRAGMA user_version", getUserVersionCallback, &userVersion);

  if (userVersion == 0) {
    ExecImplThrows(
        m_db,
        u8"CREATE TABLE IF NOT EXISTS AsyncLocalStorage(key TEXT PRIMARY KEY, value TEXT NOT NULL); PRAGMA user_version=1");
  }
}

AsyncStorageModuleWin32::~AsyncStorageModuleWin32() {
  sqlite3_close(m_db);
}

std::string AsyncStorageModuleWin32::getName() {
  return "AsyncLocalStorage";
}

std::map<std::string, dynamic> AsyncStorageModuleWin32::getConstants() {
  return {};
}

std::vector<facebook::xplat::module::CxxModule::Method> AsyncStorageModuleWin32::getMethods() {
  return {Method("multiGet", this, &AsyncStorageModuleWin32::multiGet),
          Method("multiSet", this, &AsyncStorageModuleWin32::multiSet),
          Method("multiRemove", this, &AsyncStorageModuleWin32::multiRemove),
          Method("clear", this, &AsyncStorageModuleWin32::clear),
          Method("getAllKeys", this, &AsyncStorageModuleWin32::getAllKeys)};
}

void AsyncStorageModuleWin32::multiGet(folly::dynamic params, Callback callback) {
  folly::dynamic result = folly::dynamic::array;
  auto &&args = params[0];
  if (args.size() == 0) {
    callback({{}, result});
    return;
  }
  if (!CheckSize(m_db, args, callback)) {
    return;
  }
  auto argCount = static_cast<int>(args.size());
  auto sql = MakeSQLiteParameterizedStatement(u8"SELECT key, value FROM AsyncLocalStorage WHERE key IN ", argCount);
  auto pStmt = PrepareStatement(sql.data(), callback);
  if (!pStmt) {
    return;
  }
  for (int i = 0; i < argCount; i++) {
    // todo check that arg is a string?
    if (!BindString(pStmt, i + 1, args[i].getString(), callback))
      return;
  }
  for (auto stepResult = sqlite3_step(pStmt.get()); stepResult != SQLITE_DONE; stepResult = sqlite3_step(pStmt.get())) {
    if (stepResult != SQLITE_ROW) {
      InvokeError(callback, sqlite3_errmsg(m_db));
      return;
    }

    auto key = reinterpret_cast<const char *>(sqlite3_column_text(pStmt.get(), 0));
    if (!key) {
      InvokeError(callback, sqlite3_errmsg(m_db));
      return;
    }
    auto value = reinterpret_cast<const char *>(sqlite3_column_text(pStmt.get(), 1));
    if (!value) {
      InvokeError(callback, sqlite3_errmsg(m_db));
      return;
    }
    result.push_back(folly::dynamic::array(key, value));
  }
  callback({{}, result});
}

void AsyncStorageModuleWin32::multiSet(folly::dynamic params, Callback callback) {
  auto &&args = params[0];
  if (args.size() == 0) {
    callback({});
    return;
  }
  Sqlite3Transaction transaction(m_db, callback);
  if (!transaction) {
    return;
  }
  auto pStmt = PrepareStatement(u8"INSERT OR REPLACE INTO AsyncLocalStorage VALUES(?, ?)", callback);
  if (!pStmt) {
    return;
  }
  for (auto &&arg : args) {
    if (!BindString(pStmt, 1, arg[0].getString(), callback) || !BindString(pStmt, 2, arg[1].getString(), callback)) {
      return;
    }
    auto rc = sqlite3_step(pStmt.get());
    if (rc != SQLITE_DONE && !CheckSQLiteResult(rc, callback)) {
      return;
    }
    if (!CheckSQLiteResult(sqlite3_reset(pStmt.get()), callback)) {
      return;
    }
  }
  if (!transaction.Commit()) {
    return;
  }
  callback({});
} // namespace react

void AsyncStorageModuleWin32::multiRemove(folly::dynamic params, Callback callback) {
  auto &&args = params[0];
  if (args.size() == 0) {
    callback({});
    return;
  }
  if (!CheckSize(m_db, args, callback)) {
    return;
  }
  auto argCount = static_cast<int>(args.size());
  auto sql = MakeSQLiteParameterizedStatement(u8"DELETE FROM AsyncLocalStorage WHERE key IN ", argCount);
  auto pStmt = PrepareStatement(sql.data(), callback);
  if (!pStmt) {
    return;
  }
  for (auto stepResult = sqlite3_step(pStmt.get()); stepResult != SQLITE_DONE; stepResult = sqlite3_step(pStmt.get())) {
    if (stepResult != SQLITE_ROW) {
      InvokeError(callback, sqlite3_errmsg(m_db));
      return;
    }
  }
  callback({});
}

void AsyncStorageModuleWin32::clear(folly::dynamic, Callback callback) {
  if (Exec(u8"DELETE FROM AsyncLocalStorage", callback)) {
    callback({});
  }
}

void AsyncStorageModuleWin32::getAllKeys(folly::dynamic, Callback callback) {
  folly::dynamic jsRetVal = folly::dynamic::array;
  auto getAllKeysCallback = [&](int cCol, char **rgszColText, char **) {
    if (cCol >= 1) {
      jsRetVal.push_back(rgszColText[0]);
    }
    return SQLITE_OK;
  };

  if (Exec(u8"SELECT key FROM AsyncLocalStorage", callback, getAllKeysCallback)) {
    callback({{}, jsRetVal});
  }
}

AsyncStorageModuleWin32::Statement AsyncStorageModuleWin32::PrepareStatement(const char *stmt, Callback &callback) {
  sqlite3_stmt *pStmt{nullptr};
  if (!CheckSQLiteResult(sqlite3_prepare_v2(m_db, stmt, -1, &pStmt, nullptr), callback)) {
    return {nullptr, sqlite3_finalize};
  }
  return {pStmt, &sqlite3_finalize};
}

bool AsyncStorageModuleWin32::Exec(const char *statement, ExecCallback execCallback, void *pv, Callback &callback) {
  return ExecImpl(m_db, statement, execCallback, pv, callback);
}

bool AsyncStorageModuleWin32::BindString(const Statement &stmt, int index, const std::string &str, Callback &callback) {
  return CheckSQLiteResult(sqlite3_bind_text(stmt.get(), index, str.c_str(), -1, SQLITE_TRANSIENT), callback);
}

bool AsyncStorageModuleWin32::CheckSQLiteResult(int sqliteResult, Callback &callback) {
  if (sqliteResult == SQLITE_OK) {
    return true;
  } else {
    InvokeError(callback, sqlite3_errmsg(m_db));
    return false;
  }
}

} // namespace react
} // namespace facebook

namespace react {
namespace windows {

REACTWINDOWS_API_(void) SetAsyncStorageDBPath(std::string &&dbPath) {
  if (!AsyncStorageDBPath().empty())
    throw std::logic_error("AsyncStorageDBPath already set");

  AsyncStorageDBPath() = std::move(dbPath);
}

} // namespace windows
} // namespace react
