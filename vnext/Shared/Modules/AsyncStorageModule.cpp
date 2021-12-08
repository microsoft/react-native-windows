// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AsyncStorageModule.h"

#undef U

using facebook::xplat::module::CxxModule;
using std::map;
using std::string;
using std::vector;
using std::weak_ptr;

namespace {
constexpr char moduleName[] = "AsyncLocalStorage";
}

namespace facebook {
namespace react {
AsyncStorageModule::AsyncStorageModule(const WCHAR *storageFileName)
    : m_asyncStorageManager{std::make_shared<AsyncStorageManager>(storageFileName)} {}

string AsyncStorageModule::getName() {
  return moduleName;
}

map<string, dynamic> AsyncStorageModule::getConstants() {
  return {};
}

vector<CxxModule::Method> AsyncStorageModule::getMethods() {
  return {
      {"multiGet",
       [wpManager = weak_ptr<AsyncStorageManager>(m_asyncStorageManager)](
           dynamic args,
           Callback jsCallback) // params - array<std::string> Keys ,
                                // Callback(error, returnValue)
       {
         if (auto spManager = wpManager.lock()) {
           spManager->executeKVOperation(AsyncStorageManager::AsyncStorageOperation::multiGet, args, jsCallback);
         }
       }},

      {"multiSet",
       [wpManager = weak_ptr<AsyncStorageManager>(m_asyncStorageManager)](
           dynamic args,
           Callback jsCallback) // params - array<array<std::string>>
                                // KeyValuePairs , Callback(error)
       {
         if (auto spManager = wpManager.lock()) {
           spManager->executeKVOperation(AsyncStorageManager::AsyncStorageOperation::multiSet, args, jsCallback);
         }
       }},

      // The 'multiMerge' method is currently not implemented. We assume that
      // the ReactNative framework translates its absence into the AsyncStorage
      // JS object (see Libraries\Storage\AsyncStorage.js around line 485) which
      // in turn allows JS user code to test for support of merge methods.

      {"multiRemove",
       [wpManager = weak_ptr<AsyncStorageManager>(m_asyncStorageManager)](
           dynamic args, Callback jsCallback) // params - array<std::string>
                                              // Keys , Callback(error)
       {
         if (auto spManager = wpManager.lock()) {
           spManager->executeKVOperation(AsyncStorageManager::AsyncStorageOperation::multiRemove, args, jsCallback);
         }
       }},

      {"clear",
       [wpManager = weak_ptr<AsyncStorageManager>(m_asyncStorageManager)](
           dynamic args,
           Callback jsCallback) // params - args is unused , Callback(error)
       {
         if (auto spManager = wpManager.lock()) {
           spManager->executeKVOperation(AsyncStorageManager::AsyncStorageOperation::clear, args, jsCallback);
         }
       }},

      {"getAllKeys",
       [wpManager = weak_ptr<AsyncStorageManager>(m_asyncStorageManager)](
           dynamic args, Callback jsCallback) // params - args is unused ,
                                              // Callback(error, returnValue)
       {
         if (auto spManager = wpManager.lock()) {
           spManager->executeKVOperation(AsyncStorageManager::AsyncStorageOperation::getAllKeys, args, jsCallback);
         }
       }},
  };
}

std::unique_ptr<CxxModule> CreateAsyncStorageModule(const WCHAR *storageFileName) noexcept {
  return std::make_unique<AsyncStorageModule>(storageFileName);
}

} // namespace react
} // namespace facebook
