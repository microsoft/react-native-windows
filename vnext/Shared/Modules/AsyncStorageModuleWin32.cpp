// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AsyncStorageModuleWin32.h"
#include "AsyncStorageModuleWin32Config.h"

#include <cstdio>

/// Implements AsyncStorageModule using winsqlite3.dll (requires Windows version 10.0.10586)

using facebook::react::AsyncStorageModuleWin32;
using facebook::xplat::module::CxxModule;

namespace {

std::string &AsyncStorageDBPath() {
  static std::string asyncStorageDBPath;
  return asyncStorageDBPath;
}

void InvokeError(const CxxModule::Callback &callback, const char *message) {
  callback({folly::dynamic::object("message", message)});
}

using ExecCallback = int(SQLITE_CALLBACK *)(void *, int, char **, char **);

// Execute the provided SQLite statement (and optional execCallback & user data
// in pv). On error, throw a runtime_error with the SQLite error message
void Exec(sqlite3 *db, const char *statement, ExecCallback execCallback = nullptr, void *pv = nullptr) {
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

// Execute the provided SQLite statement (and optional execCallback & user data
// in pv). On error, reports it to the callback and returns false.
bool Exec(
    sqlite3 *db,
    const CxxModule::Callback &callback,
    const char *statement,
    ExecCallback execCallback = nullptr,
    void *pv = nullptr) {
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

// Convenience wrapper for using Exec with lambda expressions
template <class Fn>
bool Exec(sqlite3 *db, const CxxModule::Callback &callback, const char *statement, Fn &fn) {
  return Exec(
      db,
      callback,
      statement,
      [](void *pv, int i, char **x, char **y) { return (*static_cast<Fn *>(pv))(i, x, y); },
      &fn);
}

// Checks that the args parameter is an array, that args.size() is less than
// SQLITE_LIMIT_VARIABLE_NUMBER, and that every member of args is a string.
// Invokes callback to report an error and returns false.
bool CheckArgs(sqlite3 *db, folly::dynamic &args, const CxxModule::Callback &callback) {
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
  for (int i = 0; i < static_cast<int>(argCount); i++) {
    if (!args[i].isString()) {
      InvokeError(callback, "Invalid key type. Expected a string");
    }
  }
  return true;
}

// RAII object to manage SQLite transaction. On destruction, if
// Commit() has not been called, rolls back the transactions
// The provided sqlite connection handle & Callback must outlive
// the Sqlite3Transaction object
class Sqlite3Transaction final {
  sqlite3 *m_db{nullptr};
  const CxxModule::Callback *m_callback{nullptr};

 public:
  Sqlite3Transaction() = default;
  Sqlite3Transaction(sqlite3 *db, const CxxModule::Callback &callback) : m_db(db), m_callback(&callback) {
    if (!Exec(m_db, *m_callback, u8"BEGIN TRANSACTION")) {
      m_db = nullptr;
      m_callback = nullptr;
    }
  }
  Sqlite3Transaction(const Sqlite3Transaction &) = delete;
  Sqlite3Transaction(Sqlite3Transaction &&other) noexcept : m_db(other.m_db), m_callback(other.m_callback) {
    other.m_db = nullptr;
    other.m_callback = nullptr;
  }
  Sqlite3Transaction &operator=(const Sqlite3Transaction &) = delete;
  Sqlite3Transaction &operator=(Sqlite3Transaction &&rhs) noexcept {
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
      Exec(m_db, *m_callback, u8"ROLLBACK");
      m_db = nullptr;
      m_callback = nullptr;
    }
  }

  bool Commit() {
    if (!m_db) {
      return false;
    }
    auto result = Exec(m_db, *m_callback, u8"COMMIT");
    m_db = nullptr;
    m_callback = nullptr;
    return result;
  }

  ~Sqlite3Transaction() {
    Rollback();
  }
};

// Appends argcount variables to prefix in a comma-separated list.
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

// Checks if sqliteResult is SQLITE_OK. If not, reports the error via
// callback & returns false.
bool CheckSQLiteResult(sqlite3 *db, const CxxModule::Callback &callback, int sqliteResult) {
  if (sqliteResult == SQLITE_OK) {
    return true;
  } else {
    InvokeError(callback, sqlite3_errmsg(db));
    return false;
  }
}

using Statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

// Creates a prepared SQLite statement. On error, returns nullptr
Statement PrepareStatement(sqlite3 *db, const CxxModule::Callback &callback, const char *stmt) {
  sqlite3_stmt *pStmt{nullptr};
  if (!CheckSQLiteResult(db, callback, sqlite3_prepare_v2(db, stmt, -1, &pStmt, nullptr))) {
    return {nullptr, sqlite3_finalize};
  }
  return {pStmt, &sqlite3_finalize};
}

// Binds the index-th variable in this prepared statement to str.
bool BindString(
    sqlite3 *db,
    const CxxModule::Callback &callback,
    const Statement &stmt,
    int index,
    const std::string &str) {
  return CheckSQLiteResult(db, callback, sqlite3_bind_text(stmt.get(), index, str.c_str(), -1, SQLITE_TRANSIENT));
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

  Exec(m_db, u8"PRAGMA user_version", getUserVersionCallback, &userVersion);

  if (userVersion == 0) {
    Exec(
        m_db,
        u8"CREATE TABLE IF NOT EXISTS AsyncLocalStorage(key TEXT PRIMARY KEY, value TEXT NOT NULL); PRAGMA user_version=1");
  }
}

AsyncStorageModuleWin32::~AsyncStorageModuleWin32() {
  decltype(m_tasks) tasks;
  {
    // If there is an in-progress async task, cancel it and wait on the
    // condition_variable for the async task to acknowledge cancellation by
    // nulling out m_action. Once m_action is null, it is safe to proceed
    // wth closing the DB connection
    winrt::slim_lock_guard guard{m_lock};
    swap(tasks, m_tasks);
    if (m_action) {
      m_action.Cancel();
      m_cv.wait(m_lock, [this]() { return m_action == nullptr; });
    }
  }
  sqlite3_close(m_db);
}

std::string AsyncStorageModuleWin32::getName() {
  return "AsyncLocalStorage";
}

std::map<std::string, dynamic> AsyncStorageModuleWin32::getConstants() {
  return {};
}

std::vector<CxxModule::Method> AsyncStorageModuleWin32::getMethods() {
  return {Method("multiGet", this, &AsyncStorageModuleWin32::multiGet),
          Method("multiSet", this, &AsyncStorageModuleWin32::multiSet),
          Method("multiRemove", this, &AsyncStorageModuleWin32::multiRemove),
          Method("clear", this, &AsyncStorageModuleWin32::clear),
          Method("getAllKeys", this, &AsyncStorageModuleWin32::getAllKeys)};
}

void AsyncStorageModuleWin32::multiGet(folly::dynamic args, Callback jsCallback) {
  auto &keys = args[0];
  if (keys.size() == 0) {
    jsCallback({{}, {}});
    return;
  }
  AddTask(DBTask::Type::multiGet, std::move(keys), std::move(jsCallback));
}
void AsyncStorageModuleWin32::multiSet(folly::dynamic args, Callback jsCallback) {
  auto &kvps = args[0];
  if (kvps.size() == 0) {
    jsCallback({});
    return;
  }
  AddTask(DBTask::Type::multiSet, std::move(kvps), std::move(jsCallback));
}
void AsyncStorageModuleWin32::multiRemove(folly::dynamic args, Callback jsCallback) {
  auto &keys = args[0];
  if (keys.size() == 0) {
    jsCallback({{}, {}});
    return;
  }
  AddTask(DBTask::Type::multiRemove, std::move(keys), std::move(jsCallback));
}
void AsyncStorageModuleWin32::clear(folly::dynamic, Callback jsCallback) {
  AddTask(DBTask::Type::clear, std::move(jsCallback));
}
void AsyncStorageModuleWin32::getAllKeys(folly::dynamic, Callback jsCallback) {
  AddTask(DBTask::Type::getAllKeys, std::move(jsCallback));
}

// Under the lock, add a task to m_tasks and, if no async task is in progress,
// schedule it
void AsyncStorageModuleWin32::AddTask(
    AsyncStorageModuleWin32::DBTask::Type type,
    folly::dynamic &&args,
    Callback &&jsCallback) {
  winrt::slim_lock_guard guard(m_lock);
  m_tasks.emplace_back(type, std::move(args), std::move(jsCallback));
  if (!m_action)
    m_action = RunTasks();
}

// On a background thread, while the async task  has not been cancelled and
// there are more tasks to do, run the tasks. When there are either no more
// tasks or cancellation has been requested, set m_action to null to report
// that and complete the coroutine. N.B., it is important that detecting that
// m_tasks is empty and acknowledging completion is done atomically; otherwise
// there would be a race between the background task detecting m_tasks.empty()
// and AddTask checking the coroutine is running.
winrt::Windows::Foundation::IAsyncAction AsyncStorageModuleWin32::RunTasks() {
  auto cancellationToken = co_await winrt::get_cancellation_token();
  co_await winrt::resume_background();
  while (!cancellationToken()) {
    decltype(m_tasks) tasks;
    sqlite3 *db{nullptr};
    {
      winrt::slim_lock_guard guard(m_lock);
      if (m_tasks.empty()) {
        m_action = nullptr;
        m_cv.notify_all();
        co_return;
      }
      std::swap(tasks, m_tasks);
      db = m_db;
    }

    for (auto &task : tasks) {
      task(db);
      if (cancellationToken())
        break;
    }
  }
  winrt::slim_lock_guard guard(m_lock);
  m_action = nullptr;
  m_cv.notify_all();
}

void AsyncStorageModuleWin32::DBTask::operator()(sqlite3 *db) {
  switch (m_type) {
    case Type::multiGet:
      multiGet(db);
      break;
    case Type::multiSet:
      multiSet(db);
      break;
    case Type::multiRemove:
      multiRemove(db);
      break;
    case Type::clear:
      clear(db);
      break;
    case Type::getAllKeys:
      getAllKeys(db);
      break;
  }
}

void AsyncStorageModuleWin32::DBTask::multiGet(sqlite3 *db) {
  folly::dynamic result = folly::dynamic::array;
  if (!CheckArgs(db, m_args, m_callback)) {
    return;
  }

  auto argCount = static_cast<int>(m_args.size());
  auto sql = MakeSQLiteParameterizedStatement(u8"SELECT key, value FROM AsyncLocalStorage WHERE key IN ", argCount);
  auto pStmt = PrepareStatement(db, m_callback, sql.data());
  if (!pStmt) {
    return;
  }
  for (int i = 0; i < argCount; i++) {
    if (!BindString(db, m_callback, pStmt, i + 1, m_args[i].getString()))
      return;
  }
  for (auto stepResult = sqlite3_step(pStmt.get()); stepResult != SQLITE_DONE; stepResult = sqlite3_step(pStmt.get())) {
    if (stepResult != SQLITE_ROW) {
      InvokeError(m_callback, sqlite3_errmsg(db));
      return;
    }

    auto key = reinterpret_cast<const char *>(sqlite3_column_text(pStmt.get(), 0));
    if (!key) {
      InvokeError(m_callback, sqlite3_errmsg(db));
      return;
    }
    auto value = reinterpret_cast<const char *>(sqlite3_column_text(pStmt.get(), 1));
    if (!value) {
      InvokeError(m_callback, sqlite3_errmsg(db));
      return;
    }
    result.push_back(folly::dynamic::array(key, value));
  }
  m_callback({{}, result});
}

void AsyncStorageModuleWin32::DBTask::multiSet(sqlite3 *db) {
  Sqlite3Transaction transaction(db, m_callback);
  if (!transaction) {
    return;
  }
  auto pStmt = PrepareStatement(db, m_callback, u8"INSERT OR REPLACE INTO AsyncLocalStorage VALUES(?, ?)");
  if (!pStmt) {
    return;
  }
  for (auto &&arg : m_args) {
    if (!BindString(db, m_callback, pStmt, 1, arg[0].getString()) ||
        !BindString(db, m_callback, pStmt, 2, arg[1].getString())) {
      return;
    }
    auto rc = sqlite3_step(pStmt.get());
    if (rc != SQLITE_DONE && !CheckSQLiteResult(db, m_callback, rc)) {
      return;
    }
    if (!CheckSQLiteResult(db, m_callback, sqlite3_reset(pStmt.get()))) {
      return;
    }
  }
  if (!transaction.Commit()) {
    return;
  }
  m_callback({});
}

void AsyncStorageModuleWin32::DBTask::multiRemove(sqlite3 *db) {
  if (!CheckArgs(db, m_args, m_callback)) {
    return;
  }

  auto argCount = static_cast<int>(m_args.size());
  auto sql = MakeSQLiteParameterizedStatement(u8"DELETE FROM AsyncLocalStorage WHERE key IN ", argCount);
  auto pStmt = PrepareStatement(db, m_callback, sql.data());
  if (!pStmt) {
    return;
  }
  for (int i = 0; i < argCount; i++) {
    if (!BindString(db, m_callback, pStmt, i + 1, m_args[i].getString()))
      return;
  }
  for (auto stepResult = sqlite3_step(pStmt.get()); stepResult != SQLITE_DONE; stepResult = sqlite3_step(pStmt.get())) {
    if (stepResult != SQLITE_ROW) {
      InvokeError(m_callback, sqlite3_errmsg(db));
      return;
    }
  }
  m_callback({});
}

void AsyncStorageModuleWin32::DBTask::clear(sqlite3 *db) {
  if (Exec(db, m_callback, u8"DELETE FROM AsyncLocalStorage")) {
    m_callback({});
  }
}

void AsyncStorageModuleWin32::DBTask::getAllKeys(sqlite3 *db) {
  folly::dynamic result = folly::dynamic::array;
  auto getAllKeysCallback = [&](int cCol, char **rgszColText, char **) {
    if (cCol >= 1) {
      result.push_back(rgszColText[0]);
    }
    return SQLITE_OK;
  };

  if (Exec(db, m_callback, u8"SELECT key FROM AsyncLocalStorage", getAllKeysCallback)) {
    m_callback({{}, result});
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
