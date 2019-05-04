// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <AsyncStorage/AsyncStorageManager.h>

using namespace std;
using namespace folly;
using namespace facebook::xplat;

namespace facebook {
  namespace react {
    const folly::dynamic noError;
    const std::vector<folly::dynamic> noErrorVector = { noError };

    AsyncStorageManager::AsyncStorageManager(const WCHAR* storageFileName) :
      m_aofKVStorage{ make_unique<KeyValueStorage>(storageFileName) },
      m_stopConsumer{ false },
      m_consumerTask{ std::async(std::launch::async, &AsyncStorageManager::consumeSetRequest, this) }
    {
    }

    AsyncStorageManager::~AsyncStorageManager()
    {
      m_stopConsumer = true;
      m_storageQueueConditionVariable.notify_one();
      m_consumerTask.get();
    }

    void AsyncStorageManager::putRequestOnQueue(AsyncStorageOperation operation, const dynamic& args, const module::CxxModule::Callback& jsCallback) noexcept
    {
      std::unique_ptr<AsyncRequestQueueArguments> arguments = make_unique<AsyncRequestQueueArguments>(operation, std::move(args), std::move(jsCallback));
      {
        std::lock_guard<std::mutex> lockGuard(m_setQueueMutex);
        m_asyncQueue.push(std::move(arguments));
      }

      m_storageQueueConditionVariable.notify_one();
    }

    void AsyncStorageManager::consumeSetRequest() noexcept
    {
      while (!m_stopConsumer)
      {
        std::unique_lock<std::mutex> uniqueMutex(m_setQueueMutex);
        m_storageQueueConditionVariable.wait(uniqueMutex, [this] {return m_stopConsumer || !m_asyncQueue.empty(); });

        if (!m_asyncQueue.empty())
        {
          std::unique_ptr<AsyncRequestQueueArguments> arguments = std::move(m_asyncQueue.front());
          m_asyncQueue.pop();

          uniqueMutex.unlock();

          executeAsyncKVOperation(arguments->m_operation, arguments->m_args, arguments->m_jsCallback);
        }
      }
    }

    folly::dynamic AsyncStorageManager::makeError(std::string&& strErrorMessage) noexcept
    {
      folly::dynamic error = folly::dynamic::object("message", strErrorMessage);
      return{ error };
    }

    void AsyncStorageManager::executeKVOperation(AsyncStorageOperation operation, const dynamic& args, const module::CxxModule::Callback& jsCallback) noexcept
    {
      try
      {
        switch (operation)
        {
        case AsyncStorageOperation::multiGet:
          multiGetInternal(args, jsCallback);
          break;

        case AsyncStorageOperation::getAllKeys:
          getAllKeysInternal(args, jsCallback);
          break;

        default:
          putRequestOnQueue(operation, args, jsCallback);
          break;
        }
      }
      catch (std::exception &e)
      {
        jsCallback({ makeError(e.what()) });
      }
    }

    void AsyncStorageManager::executeAsyncKVOperation(AsyncStorageOperation operation, const dynamic& args, const module::CxxModule::Callback& jsCallback) noexcept
    {
      try
      {
        switch (operation)
        {
        case AsyncStorageOperation::multiSet:
          multiSetInternal(args, jsCallback);
          break;

        case AsyncStorageOperation::multiRemove:
          multiRemoveInternal(args, jsCallback);
          break;

        case AsyncStorageOperation::clear:
          clearInternal(args, jsCallback);
          break;

        case AsyncStorageOperation::multiMerge:
          multiMergeInternal(args, jsCallback);
          break;

        default:
          [this, jsCallback] {jsCallback({ makeError("Invalid AsyncStorage operation") }); };
          break;
        }
      }
      catch (std::exception &e)
      {
        jsCallback({ makeError(e.what()) });
      }
    }

    void AsyncStorageManager::multiGetInternal(const dynamic& args, const module::CxxModule::Callback& jsCallback)
    {
      std::vector<std::tuple<std::string, std::string>> retVals = m_aofKVStorage->multiGet(FollyDynamicConverter::jsArgAsStringVector(args));
      folly::dynamic jsRetVal = FollyDynamicConverter::tupleStringVectorAsRetVal(retVals);
      jsCallback({ noError, jsRetVal });
    }

    void AsyncStorageManager::multiSetInternal(const dynamic& args, const module::CxxModule::Callback& jsCallback)
    {
      m_aofKVStorage->multiSet(FollyDynamicConverter::jsArgAsTupleStringVector(args));
      jsCallback(noErrorVector);
    }

    void AsyncStorageManager::multiRemoveInternal(const dynamic& args, const module::CxxModule::Callback& jsCallback)
    {
      m_aofKVStorage->multiRemove(FollyDynamicConverter::jsArgAsStringVector(args));
      jsCallback(noErrorVector);
    }

    void AsyncStorageManager::clearInternal(const dynamic& args, const module::CxxModule::Callback& jsCallback)
    {
      UNREFERENCED_PARAMETER(args);
      m_aofKVStorage->clear();
      jsCallback(noErrorVector);
    }

    void AsyncStorageManager::multiMergeInternal(const dynamic& args, const module::CxxModule::Callback& jsCallback)
    {
      m_aofKVStorage->multiMerge(FollyDynamicConverter::jsArgAsTupleStringVector(args));
      jsCallback(noErrorVector);
    }

    void AsyncStorageManager::getAllKeysInternal(const dynamic& args, const module::CxxModule::Callback& jsCallback)
    {
      std::vector<std::string> keys = m_aofKVStorage->getAllKeys();
      if (!keys.empty())
      {
        folly::dynamic jsRetVal = FollyDynamicConverter::stringVectorAsRetVal(keys);
        jsCallback({ noError, jsRetVal });
      }
      else
      {
        jsCallback({ "AsyncStorageError - No Keys Found",{} });
      }
    }
  }
}
