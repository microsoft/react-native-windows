#include "pch.h"

#include "AsyncStorageModuleWin32.h"

#include <cstdio>

/// Implements ASM using winsqlite3.dll (requires Windows version 10.0.10586)

namespace facebook {
namespace react {

namespace {

static void ExecImpl(sqlite3 *db, const char *statement, AsyncStorageModuleWin32::ExecCallback callback, void *pv) {
  char *errMsg = nullptr;
  int rc = sqlite3_exec(db, statement, callback, pv, &errMsg);
  if (errMsg) {
    throw std::runtime_error(errMsg);
  }
  if (rc != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db));
  }
}

class Sqlite3Transaction {
  sqlite3 *m_db{nullptr};

  void clear() {
    if (m_db) {
      ExecImpl(m_db, u8"COMMIT", nullptr, nullptr);
      m_db = nullptr;
    }
  }

 public:
  Sqlite3Transaction() = default;
  explicit Sqlite3Transaction(sqlite3 *db) : m_db(db) {
    if (db) {
      ExecImpl(db, u8"BEGIN TRANSACTION", nullptr, nullptr);
    }
  }
  Sqlite3Transaction(const Sqlite3Transaction &) = delete;
  Sqlite3Transaction(Sqlite3Transaction &&other) : m_db{other.m_db} {
    other.m_db = nullptr;
  }
  Sqlite3Transaction &operator=(const Sqlite3Transaction &) = delete;
  Sqlite3Transaction &operator=(Sqlite3Transaction &&rhs) {
    if (this != &rhs) {
      clear();
      std::swap(m_db, rhs.m_db);
    }
    return *this;
  }
  ~Sqlite3Transaction() {
    clear();
  }
};

} // namespace

AsyncStorageModuleWin32::AsyncStorageModuleWin32(PCSTR dbPath) {
  checkSQLiteResult(
      sqlite3_open_v2(dbPath, &m_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr));

  int userVersion = 0;
  auto userVersionCallback = [&](int cCol, char **rgszColText, char **rgszColName) {
    userVersion = atoi(rgszColText[0]);
    return SQLITE_OK;
  };

  Exec(u8"PRAGMA user_version", userVersionCallback);

  if (userVersion == 0) {
    Exec(u8"CREATE TABLE IF NOT EXISTS AsyncLocalStorage(key TEXT, value TEXT); PRAGMA user_version=1");
  }
}

AsyncStorageModuleWin32::~AsyncStorageModuleWin32() {
  checkSQLiteResult(sqlite3_close(m_db));
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
          // The 'multiMerge' method is currently not implemented. We assume that
          // the ReactNative framework translates its absence into the AsyncStorage
          // JS object (see Libraries\Storage\AsyncStorage.js around line 485) which
          // in turn allows JS user code to test for support of merge methods.
          Method("clear", this, &AsyncStorageModuleWin32::clear),
          Method("getAllKeys", this, &AsyncStorageModuleWin32::getAllKeys)};
}

void AsyncStorageModuleWin32::multiGet(folly::dynamic params, Callback callback) {
  folly::dynamic result = folly::dynamic::array;
  auto &&args = params[0];
  if (args.size() != 0) {
    Sqlite3Transaction transaction(m_db);
    auto pStmt = PrepareStatement(u8"SELECT key, value FROM AsyncLocalStorage WHERE key = ?");
    for (auto &&arg : args) {
      BindString(pStmt, 1, arg.getString());
      checkSQLiteResult(sqlite3_step(pStmt.get()));
      auto key = reinterpret_cast<const char *>(sqlite3_column_text(pStmt.get(), 0));
      if (!key)
        checkSQLiteResult(sqlite3_errcode(m_db));
      auto value = reinterpret_cast<const char *>(sqlite3_column_text(pStmt.get(), 1));
      if (!value)
        checkSQLiteResult(sqlite3_errcode(m_db));
      result.push_back(folly::dynamic::array(std::string(key), std::string(value)));
      checkSQLiteResult(sqlite3_reset(pStmt.get()));
    }
  }
  callback({folly::dynamic(), result});
}

void AsyncStorageModuleWin32::multiSet(folly::dynamic params, Callback callback) {
  auto &&args = params[0];
  if (args.size() != 0) {
    Sqlite3Transaction transaction(m_db);
    auto pStmt = PrepareStatement(u8"INSERT OR REPLACE INTO AsyncLocalStorage VALUES(?, ?)");
    for (auto &&arg : args) {
      BindString(pStmt, 1, arg[0].getString());
      BindString(pStmt, 2, arg[1].getString());
      checkSQLiteResult(sqlite3_step(pStmt.get()));
      checkSQLiteResult(sqlite3_reset(pStmt.get()));
    }
  }
  callback({});
}

void AsyncStorageModuleWin32::multiRemove(folly::dynamic params, Callback callback) {
  auto &&args = params[0];
  if (args.size() != 0) {
    Sqlite3Transaction transaction(m_db);
    auto pStmt = PrepareStatement(u8"DELETE FROM AsyncLocalStorage WHERE key = ?");
    for (auto &&arg : args) {
      checkSQLiteResult(sqlite3_bind_text(pStmt.get(), 1, arg.getString().c_str(), -1, SQLITE_TRANSIENT));
      checkSQLiteResult(sqlite3_step(pStmt.get()));
      checkSQLiteResult(sqlite3_reset(pStmt.get()));
    }
  }
  callback({});
}

void AsyncStorageModuleWin32::clear(folly::dynamic, Callback callback) {
  Exec(u8"DELETE FROM AsyncLocalStorage");
  callback({});
}

void AsyncStorageModuleWin32::getAllKeys(folly::dynamic, Callback callback) {
  folly::dynamic jsRetVal = folly::dynamic::array;
  auto getAllKeysCallback = [&](int cCol, char **rgszColText, char **) {
    if (cCol >= 1) {
      jsRetVal.push_back(rgszColText[0]);
    }
    return SQLITE_OK;
  };

  Exec(u8"SELECT key FROM AsyncLocalStorage", getAllKeysCallback);

  if (jsRetVal.size() == 0) {
    callback({"AsyncStorageError - No Keys Found", {}});
  }
}

AsyncStorageModuleWin32::Statement AsyncStorageModuleWin32::PrepareStatement(const char *stmt) {
  sqlite3_stmt *pStmt;
  checkSQLiteResult(sqlite3_prepare_v2(m_db, stmt, -1, &pStmt, nullptr));
  return {pStmt, &sqlite3_finalize};
}

void AsyncStorageModuleWin32::Exec(const char *statement, ExecCallback callback, void *pv) {
  ExecImpl(m_db, statement, callback, pv);
}

void AsyncStorageModuleWin32::BindString(const Statement &stmt, int index, const std::string &str) {
  checkSQLiteResult(sqlite3_bind_text(stmt.get(), index, str.c_str(), -1, SQLITE_TRANSIENT));
}

void AsyncStorageModuleWin32::checkSQLiteResult(int sqliteResult) {
  switch (sqliteResult) {
    case SQLITE_OK:
    case SQLITE_ROW:
    case SQLITE_DONE:
      break;
    default: {
      char errorCode[16 + 10 + 1];
      sprintf_s(errorCode, "SQLite error: %d. ", sqliteResult);
      throw std::runtime_error(std::string{errorCode} + sqlite3_errmsg(m_db));
    }
  }
}

} // namespace react
} // namespace facebook
