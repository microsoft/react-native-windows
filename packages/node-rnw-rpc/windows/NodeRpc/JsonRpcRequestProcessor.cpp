// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsonRpcRequestProcessor.h"

#include <winrt/Windows.Data.Json.h>

using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Storage::Streams;

namespace winrt::NodeRpc {

JsonRpcRequestProcessor::JsonRpcRequestProcessor(const NodeRpc::Handler &handler) noexcept : m_handler(handler) {}

winrt::fire_and_forget JsonRpcRequestProcessor::HandleRequest(
    winrt::hstring requestBody,
    IOutputStream output) noexcept {
  auto handler{m_handler};

  auto message = co_await DecodeAndValidateMessage(requestBody, output);
  if (!message) {
    co_return;
  }

  // Cannot co_await to emit failure inside catch block. Keep vars outside scope.
  winrt::hstring errorMessage;

  try {
    auto result = co_await handler.Invoke(message.GetNamedString(L"method"), message.GetNamedValue(L"params"));
    co_await EmitResult(result, message.GetNamedValue(L"id"), output);
    co_return;
  } catch (const winrt::hresult_error &ex) {
    errorMessage = ex.message();
  } catch (const std::exception &ex) {
    errorMessage = winrt::to_hstring(ex.what());
  }

  co_await EmitError(JsonRpcErrorCode::InternalError, errorMessage, message.GetNamedValue(L"id"), output);
}

IAsyncOperation<JsonObject> JsonRpcRequestProcessor::DecodeAndValidateMessage(
    winrt::hstring requestBody,
    IOutputStream output) noexcept {
  IOutputStream out(output);

  JsonObject obj;
  if (!JsonObject::TryParse(requestBody, obj)) {
    co_await EmitError(
        JsonRpcErrorCode::ParseError, L"Could not parse request JSON.", JsonValue::CreateNullValue(), out);
    co_return nullptr;
  }

  // TODO #7108 validate fields
  co_return obj;
}

IAsyncAction JsonRpcRequestProcessor::EmitError(
    JsonRpcErrorCode code,
    winrt::hstring message,
    JsonValue id,
    IOutputStream output) noexcept {
  JsonObject err;
  err.SetNamedValue(L"code", JsonValue::CreateNumberValue(static_cast<int32_t>(code)));
  err.SetNamedValue(L"message", JsonValue::CreateStringValue(message));
  co_await EmitResponse(err, id, output);
}

IAsyncAction JsonRpcRequestProcessor::EmitResult(IJsonValue result, JsonValue id, IOutputStream output) noexcept {
  JsonObject res;
  res.SetNamedValue(L"result", result);
  co_await EmitResponse(res, id, output);
}

IAsyncAction
JsonRpcRequestProcessor::EmitResponse(JsonObject responseBody, JsonValue id, IOutputStream output) noexcept {
  responseBody.SetNamedValue(L"jsonrpc", JsonValue::CreateStringValue(L"2.0"));
  responseBody.SetNamedValue(L"id", id);

  auto utf16JsonString = responseBody.Stringify();
  auto utf8JsonString = winrt::to_string(utf16JsonString);
  winrt::array_view<uint8_t> utf8JsonStringBytes(
      reinterpret_cast<uint8_t *>(utf8JsonString.data()),
      reinterpret_cast<uint8_t *>(utf8JsonString.data() + utf8JsonString.size()));

  DataWriter writer(output);
  writer.ByteOrder(ByteOrder::LittleEndian);
  writer.WriteUInt32(static_cast<uint32_t>(utf8JsonString.size()));
  writer.WriteBytes(utf8JsonStringBytes);

  co_await writer.StoreAsync();
  writer.DetachStream();
}

} // namespace winrt::NodeRpc
