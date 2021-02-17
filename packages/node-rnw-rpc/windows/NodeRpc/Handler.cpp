// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Handler.h"
#include "Handler.g.cpp"

#include <Crash.h>

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Data::Json;

namespace winrt::NodeRpc::implementation {
winrt::NodeRpc::Handler Handler::BindAction(const winrt::hstring &methodName, const RpcAction &action) noexcept {
  VerifyElseCrash(!IsMethodRegistered(methodName));
  VerifyElseCrash(!IsReservedMethodName(methodName));

  m_actionHandlers[methodName] = action;
  return NodeRpc::Handler(*this);
}

winrt::NodeRpc::Handler Handler::BindAsyncAction(
    const winrt::hstring &methodName,
    const AsyncRpcAction &action) noexcept {
  VerifyElseCrash(!IsMethodRegistered(methodName));
  VerifyElseCrash(!IsReservedMethodName(methodName));

  m_asyncActionHandlers[methodName] = action;
  return NodeRpc::Handler(*this);
}

winrt::NodeRpc::Handler Handler::BindOperation(
    const winrt::hstring &methodName,
    const RpcOperation &operation) noexcept {
  VerifyElseCrash(!IsMethodRegistered(methodName));
  VerifyElseCrash(!IsReservedMethodName(methodName));

  m_operationHandlers[methodName] = operation;
  return NodeRpc::Handler(*this);
}

winrt::NodeRpc::Handler Handler::BindAsyncOperation(
    const winrt::hstring &methodName,
    const AsyncRpcOperation &operation) noexcept {
  VerifyElseCrash(!IsMethodRegistered(methodName));
  VerifyElseCrash(!IsReservedMethodName(methodName));

  m_asyncOperationHandlers[methodName] = operation;
  return NodeRpc::Handler(*this);
}

bool Handler::IsMethodRegistered(const winrt::hstring &methodName) noexcept {
  return m_actionHandlers.count(methodName) != 0 || m_operationHandlers.count(methodName) != 0 ||
      m_asyncActionHandlers.count(methodName) != 0 || m_asyncOperationHandlers.count(methodName) != 0;
}

bool Handler::IsReservedMethodName(const winrt::hstring &methodName) noexcept {
  constexpr std::wstring_view reservedPrefix = L"rpc.";

  return std::wstring_view(methodName).compare(0, reservedPrefix.size(), reservedPrefix, 0, reservedPrefix.size()) == 0;
}

IAsyncOperation<IJsonValue> Handler::Invoke(const winrt::hstring &methodName, const JsonValue &params) {
  auto action = m_actionHandlers.find(methodName);
  if (action != m_actionHandlers.end()) {
    action->second(params);
    co_return JsonValue::CreateNullValue();
  }

  auto operation = m_operationHandlers.find(methodName);
  if (operation != m_operationHandlers.end()) {
    auto result = operation->second(params);
    co_return result;
  }

  auto asyncAction = m_asyncActionHandlers.find(methodName);
  if (asyncAction != m_asyncActionHandlers.end()) {
    co_await asyncAction->second(params);
    co_return JsonValue::CreateNullValue();
  }

  auto asyncOperation = m_asyncOperationHandlers.find(methodName);
  if (asyncOperation != m_asyncOperationHandlers.end()) {
    auto result = co_await asyncOperation->second(params);
    co_return result;
  }
}

} // namespace winrt::NodeRpc::implementation
