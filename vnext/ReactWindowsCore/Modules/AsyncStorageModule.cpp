// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "AsyncStorageModule.h"

#undef U

using namespace std;
using namespace folly;
using namespace facebook::xplat;

namespace facebook {
namespace react {
AsyncStorageModule::AsyncStorageModule(const WCHAR *storageFileName)
    : m_asyncStorageManager{make_unique<AsyncStorageManager>(storageFileName)} {}

std::string AsyncStorageModule::getName() {
  return "AsyncLocalStorage";
}

std::map<std::string, dynamic> AsyncStorageModule::getConstants() {
  return {};
}

std::vector<facebook::xplat::module::CxxModule::Method> AsyncStorageModule::getMethods() {
  return {
      Method(
          "multiGet",
          [this](
              dynamic args,
              Callback jsCallback) // params - array<std::string> Keys ,
                                   // Callback(error, returnValue)
          {
            m_asyncStorageManager->executeKVOperation(
                AsyncStorageManager::AsyncStorageOperation::multiGet, args, jsCallback);
          }),

      Method(
          "multiSet",
          [this](
              dynamic args,
              Callback jsCallback) // params - array<array<std::string>>
                                   // KeyValuePairs , Callback(error)
          {
            m_asyncStorageManager->executeKVOperation(
                AsyncStorageManager::AsyncStorageOperation::multiSet, args, jsCallback);
          }),

      // The 'multiMerge' method is currently not implemented. We assume that
      // the ReactNative framework translates its absence into the AsyncStorage
      // JS object (see Libraries\Storage\AsyncStorage.js around line 485) which
      // in turn allows JS user code to test for support of merge methods.

      Method(
          "multiRemove",
          [this](dynamic args, Callback jsCallback) // params - array<std::string>
                                                    // Keys , Callback(error)
          {
            m_asyncStorageManager->executeKVOperation(
                AsyncStorageManager::AsyncStorageOperation::multiRemove, args, jsCallback);
          }),

      Method(
          "clear",
          [this](
              dynamic args,
              Callback jsCallback) // params - args is unused , Callback(error)
          {
            m_asyncStorageManager->executeKVOperation(
                AsyncStorageManager::AsyncStorageOperation::clear, args, jsCallback);
          }),

      Method(
          "getAllKeys",
          [this](dynamic args, Callback jsCallback) // params - args is unused ,
                                                    // Callback(error, returnValue)
          {
            m_asyncStorageManager->executeKVOperation(
                AsyncStorageManager::AsyncStorageOperation::getAllKeys, args, jsCallback);
          }),
  };
}

std::unique_ptr<facebook::xplat::module::CxxModule> CreateAsyncStorageModule(const WCHAR *storageFileName) noexcept {
  return std::make_unique<AsyncStorageModule>(storageFileName);
}

} // namespace react
} // namespace facebook
