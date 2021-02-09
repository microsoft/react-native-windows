// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "Server.g.h"
#include "JsonRpcRequestProcessor.h"

#include <winrt/Windows.Networking.Sockets.h>

namespace winrt::NodeRpc::implementation {

struct Server : ServerT<Server> {
  Server(const NodeRpc::Handler &handler);
  Windows::Foundation::IAsyncAction ProcessAllClientRequests(
      uint16_t port,
      Windows::Foundation::TimeSpan pollInterval) noexcept;

 private:
  bool IsPollContinueCondition(winrt::hresult hr) noexcept;
  bool IsExpectedConnectionEnd(winrt::hresult hr) noexcept;

  Windows::Foundation::IAsyncAction PumpRequests() noexcept;

  JsonRpcRequestProcessor m_requestProcessor;
  Windows::Networking::Sockets::StreamSocket m_socket;
};

} // namespace winrt::NodeRpc::implementation

namespace winrt::NodeRpc::factory_implementation {

struct Server : ServerT<Server, implementation::Server> {};

} // namespace winrt::NodeRpc::factory_implementation
