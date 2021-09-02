// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "CommandHandler.h"
#include "CommandHandler.g.cpp"

#include <Crash.h>

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Data::Json;

namespace winrt::AutomationChannel::implementation {
winrt::AutomationChannel::CommandHandler CommandHandler::BindAction(
    const winrt::hstring &methodName,
    const SyncAction &action) noexcept {
  VerifyElseCrash(!IsMethodRegistered(methodName));
  VerifyElseCrash(!IsReservedMethodName(methodName));

  m_actionHandlers[methodName] = action;
  return AutomationChannel::CommandHandler(*this);
}

winrt::AutomationChannel::CommandHandler CommandHandler::BindAsyncAction(
    const winrt::hstring &methodName,
    const AsyncAction &action) noexcept {
  VerifyElseCrash(!IsMethodRegistered(methodName));
  VerifyElseCrash(!IsReservedMethodName(methodName));

  m_asyncActionHandlers[methodName] = action;
  return AutomationChannel::CommandHandler(*this);
}

winrt::AutomationChannel::CommandHandler CommandHandler::BindOperation(
    const winrt::hstring &methodName,
    const SyncOperation &operation) noexcept {
  VerifyElseCrash(!IsMethodRegistered(methodName));
  VerifyElseCrash(!IsReservedMethodName(methodName));

  m_operationHandlers[methodName] = operation;
  return AutomationChannel::CommandHandler(*this);
}

winrt::AutomationChannel::CommandHandler CommandHandler::BindAsyncOperation(
    const winrt::hstring &methodName,
    const AsyncOperation &operation) noexcept {
  VerifyElseCrash(!IsMethodRegistered(methodName));
  VerifyElseCrash(!IsReservedMethodName(methodName));

  m_asyncOperationHandlers[methodName] = operation;
  return AutomationChannel::CommandHandler(*this);
}

bool CommandHandler::IsMethodRegistered(const winrt::hstring &methodName) noexcept {
  return m_actionHandlers.count(methodName) != 0 || m_operationHandlers.count(methodName) != 0 ||
      m_asyncActionHandlers.count(methodName) != 0 || m_asyncOperationHandlers.count(methodName) != 0;
}

bool CommandHandler::IsReservedMethodName(const winrt::hstring &methodName) noexcept {
  constexpr std::wstring_view reservedPrefix = L"rpc.";

  return std::wstring_view(methodName).compare(0, reservedPrefix.size(), reservedPrefix, 0, reservedPrefix.size()) == 0;
}

IAsyncOperation<IJsonValue> CommandHandler::Invoke(const winrt::hstring &methodName, const JsonValue &params) {
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

} // namespace winrt::AutomationChannel::implementation
