// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <AsyncStorage/AsyncStorageManager.h>
#include <cxxreact/CxxModule.h>
#include <cxxreact/MessageQueueThread.h>
#include <folly/dynamic.h>

#include <winsqlite/winsqlite3.h>
#include <memory>

namespace facebook {
namespace react {

class AsyncStorageModuleWin32 : public facebook::xplat::module::CxxModule {
 public:
  AsyncStorageModuleWin32();
  ~AsyncStorageModuleWin32();
  std::string getName() override;
  std::map<std::string, dynamic> getConstants() override;
  std::vector<facebook::xplat::module::CxxModule::Method> getMethods() override;

  using ExecCallback = int(SQLITE_CALLBACK *)(void *, int, char **, char **);

 private:
  sqlite3 *m_db;

  // params - array<std::string> Keys , Callback(error, returnValue)
  void multiGet(folly::dynamic args, Callback jsCallback);
  // params - array<array<std::string>> KeyValuePairs , Callback(error)
  void multiSet(folly::dynamic args, Callback jsCallback);
  // params - array<std::string> Keys , Callback(error)
  void multiRemove(folly::dynamic args, Callback jsCallback);
  // params - args is unused, Callback(error)
  void clear(folly::dynamic, Callback jsCallback);
  // params - args is unused, Callback(error, returnValue)
  void getAllKeys(folly::dynamic, Callback jsCallback);

  using Statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;
  Statement PrepareStatement(const char *stmt, Callback &callback);

  bool Exec(const char *statement, ExecCallback execCallback, void *pv, Callback &callback);
  inline bool Exec(const char *statement, Callback &callback) {
    return Exec(statement, nullptr, nullptr, callback);
  }

  template <class Fn>
  bool Exec(const char *statement, Callback &callback, Fn &fn) {
    return Exec(
        statement,
        [](void *pv, int i, char **x, char **y) { return (*static_cast<Fn *>(pv))(i, x, y); },
        &fn,
        callback);
  }

  bool BindString(const Statement &stmt, int index, const std::string &str, Callback &callback);

  // On success, returns true
  // On failure, invokes callback with an appropriate object & returns false.
  bool CheckSQLiteResult(int sqliteResult, Callback &callback);

  static std::string m_dbPath;
};
} // namespace react
} // namespace facebook
