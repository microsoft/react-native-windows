// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Handler.g.h"

namespace winrt::AutomationChannel::implementation {

struct Handler : HandlerT<Handler> {
  Handler() = default;

  AutomationChannel::Handler BindAction(const winrt::hstring &methodName, const RpcAction &action) noexcept;
  AutomationChannel::Handler BindAsyncAction(const winrt::hstring &methodName, const AsyncRpcAction &action) noexcept;
  AutomationChannel::Handler BindOperation(const winrt::hstring &methodName, const RpcOperation &operation) noexcept;
  AutomationChannel::Handler BindAsyncOperation(const winrt::hstring &methodName, const AsyncRpcOperation &operation) noexcept;

  bool IsMethodRegistered(const winrt::hstring &methodName) noexcept;
  bool IsReservedMethodName(const winrt::hstring &methodName) noexcept;
  Windows::Foundation::IAsyncOperation<Windows::Data::Json::IJsonValue> Invoke(
      const winrt::hstring &methodName,
      const Windows::Data::Json::JsonValue &params);

 private:
  std::unordered_map<winrt::hstring, RpcAction> m_actionHandlers;
  std::unordered_map<winrt::hstring, RpcOperation> m_operationHandlers;
  std::unordered_map<winrt::hstring, AsyncRpcAction> m_asyncActionHandlers;
  std::unordered_map<winrt::hstring, AsyncRpcOperation> m_asyncOperationHandlers;
};

} // namespace winrt::AutomationChannel::implementation

namespace winrt::AutomationChannel::factory_implementation {

struct Handler : HandlerT<Handler, implementation::Handler> {};

} // namespace winrt::AutomationChannel::factory_implementation
