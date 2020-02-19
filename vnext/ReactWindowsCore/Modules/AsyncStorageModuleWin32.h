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
  Statement PrepareStatement(const char *stmt);

  void Exec(const char *statement, ExecCallback callback = nullptr, void *pv = nullptr);

  template <class Fn>
  void Exec(const char *statement, Fn &fn) {
    Exec(statement, [](void *pv, int i, char **x, char **y) { return (*static_cast<Fn *>(pv))(i, x, y); }, &fn);
  }

  void BindString(const Statement &stmt, int index, const std::string &str);

  // throws a std::runtime_error that includes sqlite3_errmsg if sqliteResult is an error
  void CheckSQLiteResult(int sqliteResult);

  static std::string m_dbPath;
};
} // namespace react
} // namespace facebook
