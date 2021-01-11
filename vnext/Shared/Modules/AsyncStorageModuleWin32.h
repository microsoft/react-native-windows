// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <AsyncStorage/AsyncStorageManager.h>
#include <cxxreact/CxxModule.h>
#include <cxxreact/MessageQueueThread.h>
#include <folly/dynamic.h>

#include <winrt/Windows.Foundation.h>
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

 private:
  class DBTask {
   public:
    enum class Type { multiGet, multiSet, multiRemove, clear, getAllKeys };
    DBTask(Type type, folly::dynamic &&args, Callback &&callback)
        : m_type{type}, m_args{std::move(args)}, m_callback{std::move(callback)} {}
    DBTask(const DBTask &) = delete;
    DBTask(DBTask &&) = default;
    DBTask &operator=(const DBTask &) = delete;
    DBTask &operator=(DBTask &&) = default;
    void operator()(sqlite3 *db);

   private:
    Type m_type;
    folly::dynamic m_args;
    Callback m_callback;

    void multiGet(sqlite3 *db);
    void multiSet(sqlite3 *db);
    void multiRemove(sqlite3 *db);
    void clear(sqlite3 *db);
    void getAllKeys(sqlite3 *db);
  };
  winrt::slim_mutex m_lock;
  winrt::slim_condition_variable m_cv;
  winrt::Windows::Foundation::IAsyncAction m_action{nullptr};
  std::vector<DBTask> m_tasks;
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

  void AddTask(DBTask::Type type, folly::dynamic &&args, Callback &&jsCallback);
  void AddTask(DBTask::Type type, Callback &&jsCallback) {
    AddTask(type, folly::dynamic{}, std::move(jsCallback));
  }
  winrt::Windows::Foundation::IAsyncAction RunTasks();

  static std::string m_dbPath;
};
} // namespace react
} // namespace facebook
