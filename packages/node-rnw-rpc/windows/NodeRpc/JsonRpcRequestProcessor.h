// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "Handler.g.h"

#include <winrt/Windows.Storage.Streams.h>

namespace winrt::NodeRpc {

//! Translates/verifies JSON-RPC2 messages, dispatches them, then outputs the result
class JsonRpcRequestProcessor final {
 public:
  JsonRpcRequestProcessor(const NodeRpc::Handler &handler) noexcept;

  winrt::fire_and_forget HandleRequest(
      const winrt::hstring &requestBody,
      Windows::Storage::Streams::IOutputStream output) noexcept;

 private:
  NodeRpc::Handler m_handler;
};

} // namespace winrt::NodeRpc
