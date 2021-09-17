// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "CommandHandler.g.h"

namespace winrt::AutomationChannel::implementation {

struct CommandHandler : CommandHandlerT<CommandHandler> {
  CommandHandler() = default;

  AutomationChannel::CommandHandler BindAction(const winrt::hstring &methodName, const SyncAction &action) noexcept;
  AutomationChannel::CommandHandler BindAsyncAction(
      const winrt::hstring &methodName,
      const AsyncAction &action) noexcept;
  AutomationChannel::CommandHandler BindOperation(
      const winrt::hstring &methodName,
      const SyncOperation &operation) noexcept;
  AutomationChannel::CommandHandler BindAsyncOperation(
      const winrt::hstring &methodName,
      const AsyncOperation &operation) noexcept;

  bool IsMethodRegistered(const winrt::hstring &methodName) noexcept;
  bool IsReservedMethodName(const winrt::hstring &methodName) noexcept;
  Windows::Foundation::IAsyncOperation<Windows::Data::Json::IJsonValue> Invoke(
      const winrt::hstring &methodName,
      const Windows::Data::Json::JsonValue &params);

 private:
  std::unordered_map<winrt::hstring, SyncAction> m_actionHandlers;
  std::unordered_map<winrt::hstring, SyncOperation> m_operationHandlers;
  std::unordered_map<winrt::hstring, AsyncAction> m_asyncActionHandlers;
  std::unordered_map<winrt::hstring, AsyncOperation> m_asyncOperationHandlers;
};

} // namespace winrt::AutomationChannel::implementation

namespace winrt::AutomationChannel::factory_implementation {

struct CommandHandler : CommandHandlerT<CommandHandler, implementation::CommandHandler> {};

} // namespace winrt::AutomationChannel::factory_implementation
