// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <AsyncStorage/AsyncStorageManager.h>
#include <cxxreact/CxxModule.h>
#include <cxxreact/MessageQueueThread.h>
#include <folly/dynamic.h>

#include <winsqlite/winsqlite3.h>



namespace facebook {
namespace react {

class AsyncStorageModuleWin32 : public facebook::xplat::module::CxxModule {
 public:
  explicit AsyncStorageModuleWin32(PCSTR dbPath);
  ~AsyncStorageModuleWin32();
  std::string getName() override;
  std::map<std::string, dynamic> getConstants() override;
  std::vector<facebook::xplat::module::CxxModule::Method> getMethods() override;
 private:

   static void errorLogCallback(void* pv, int iErrCode, const char* msg)
   {
     static_cast<AsyncStorageModuleWin32*>(pv)->errorLogCallbackImpl(iErrCode, msg);
   }

   void errorLogCallbackImpl(int iErrCode, const char* msg);

  sqlite3* m_dbHandle;

  // params - array<std::string> Keys , Callback(error, returnValue)
  void multiGet(folly::dynamic args, Callback jsCallback);
  // params - array<array<std::string>> KeyValuePairs , Callback(error)
  void mulitSet(folly::dynamic args, Callback jsCallback);
  // params - array<std::string> Keys , Callback(error)
  void multiRemove(folly::dynamic args, Callback jsCallback);
  // params - args is unused, Callback(error)
  void clear(folly::dynamic, Callback jsCallback);
  // params - args is unused, Callback(error, returnValue)
  void getAllKeys(folly::dynamic, Callback jsCallback);
};
} // namespace react
} // namespace facebook
