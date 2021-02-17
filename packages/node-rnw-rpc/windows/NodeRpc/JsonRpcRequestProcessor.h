// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "Handler.g.h"

#include <winrt/Windows.Storage.Streams.h>

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

//! Translates/verifies JSON-RPC2 messages, dispatches them, then outputs the result
class JsonRpcRequestProcessor final {
 public:
  JsonRpcRequestProcessor(const NodeRpc::Handler &handler) noexcept;

  winrt::fire_and_forget HandleRequest(
      winrt::hstring requestBody,
      Windows::Storage::Streams::IOutputStream output) noexcept;

 private:
  Windows::Foundation::IAsyncOperation<Windows::Data::Json::JsonObject> DecodeAndValidateMessage(
      winrt::hstring requestBody,
      Windows::Storage::Streams::IOutputStream output) noexcept;

  Windows::Foundation::IAsyncAction EmitError(
      JsonRpcErrorCode code,
      winrt::hstring message,
      Windows::Data::Json::JsonValue id,
      Windows::Storage::Streams::IOutputStream output) noexcept;
  Windows::Foundation::IAsyncAction EmitResult(
      Windows::Data::Json::IJsonValue result,
      Windows::Data::Json::JsonValue id,
      Windows::Storage::Streams::IOutputStream output) noexcept;
  Windows::Foundation::IAsyncAction EmitResponse(
      Windows::Data::Json::JsonObject responseBody,
      Windows::Data::Json::JsonValue id,
      Windows::Storage::Streams::IOutputStream output) noexcept;

  NodeRpc::Handler m_handler;
};

} // namespace winrt::NodeRpc
