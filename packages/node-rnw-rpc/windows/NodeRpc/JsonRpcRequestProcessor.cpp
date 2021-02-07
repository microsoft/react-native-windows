// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsonRpcRequestProcessor.h"

namespace winrt::NodeRpc {
//! Predefined error codes for the JSON-RPC Spec
//! See https://www.jsonrpc.org/specification
enum class JsonRpcErrorCode : int32_t {
  ParseError = -32700,
  InvalidRequest = -32600,
  MethodNotFound = -32601,
  InvalidParams = -32602,
  InternalError = -32603,
};

JsonRpcRequestProcessor::JsonRpcRequestProcessor(const NodeRpc::Handler &handler) noexcept : m_handler(handler) {}

winrt::fire_and_forget JsonRpcRequestProcessor::HandleRequest(
    const winrt::hstring &requestBody,
    Windows::Storage::Streams::IOutputStream output) noexcept {
  // TODO
}

} // namespace winrt::NodeRpc
