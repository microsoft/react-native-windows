// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <folly/dynamic.h>
#include <cxxreact/CxxModule.h>
#include <AsyncStorage/KeyValueStorage.h>
#include <AsyncStorage/FollyDynamicConverter.h>

#include <queue>
#include <future>
#include <condition_variable>

namespace facebook {
  namespace react {
    class AsyncStorageManager
    {
    public:
      AsyncStorageManager(const WCHAR* storageFileName);
      ~AsyncStorageManager();

      enum class AsyncStorageOperation { multiGet, multiSet, multiRemove, clear, multiMerge, getAllKeys };
      void executeKVOperation(AsyncStorageOperation operation, const folly::dynamic& args, const xplat::module::CxxModule::Callback& jsCallback) noexcept;

    private:
      struct AsyncRequestQueueArguments
      {
        AsyncRequestQueueArguments(AsyncStorageOperation paramOperation, const folly::dynamic&& paramArgs, const xplat::module::CxxModule::Callback&& paramJsCallback) noexcept
          : m_operation(paramOperation), m_args(std::move(paramArgs)), m_jsCallback(std::move(paramJsCallback))
        { }

        AsyncStorageOperation m_operation;
        folly::dynamic m_args;
        xplat::module::CxxModule::Callback m_jsCallback;
      };

    private:
      std::atomic_bool m_stopConsumer;
      std::mutex m_setQueueMutex;
      std::condition_variable m_storageQueueConditionVariable;
      std::future<void> m_consumerTask;
      std::queue<std::unique_ptr<AsyncStorageManager::AsyncRequestQueueArguments>> m_asyncQueue;
      std::unique_ptr<KeyValueStorage> m_aofKVStorage;

    private:
      folly::dynamic makeError(std::string&& strErrorMessage) noexcept;

      void executeAsyncKVOperation(AsyncStorageOperation operation, const folly::dynamic& args, const xplat::module::CxxModule::Callback& jsCallback) noexcept;

      void consumeSetRequest() noexcept;
      void putRequestOnQueue(AsyncStorageOperation operation, const folly::dynamic& args, const xplat::module::CxxModule::Callback& jsCallback) noexcept;

      void multiGetInternal(const folly::dynamic& args, const xplat::module::CxxModule::Callback& jsCallback);
      void multiSetInternal(const folly::dynamic& args, const xplat::module::CxxModule::Callback& jsCallback);
      void multiRemoveInternal(const folly::dynamic& args, const xplat::module::CxxModule::Callback& jsCallback);
      void clearInternal(const folly::dynamic& args, const xplat::module::CxxModule::Callback& jsCallback);
      void multiMergeInternal(const folly::dynamic& args, const xplat::module::CxxModule::Callback& jsCallback);
      void getAllKeysInternal(const folly::dynamic& args, const xplat::module::CxxModule::Callback& jsCallback);
    };
  }
}
