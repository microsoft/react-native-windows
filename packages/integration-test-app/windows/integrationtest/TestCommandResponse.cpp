// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TestCommandResponse.h"

using namespace winrt::Windows::Data::Json;

namespace IntegrationTest {

JsonObject TestCommandResponse::Okay() noexcept {
  JsonObject responseJson;
  responseJson.SetNamedValue(L"status", JsonValue::CreateStringValue(L"okay"));
  return responseJson;
}

JsonObject TestCommandResponse::Timeout() noexcept {
  JsonObject responseJson;
  responseJson.SetNamedValue(L"status", JsonValue::CreateStringValue(L"timeout"));
  return responseJson;
}

JsonObject TestCommandResponse::TestPassed() noexcept {
  JsonObject responseJson;
  responseJson.SetNamedValue(L"status", JsonValue::CreateStringValue(L"passed"));
  return responseJson;
}

JsonObject TestCommandResponse::TestFailed() noexcept {
  JsonObject responseJson;
  responseJson.SetNamedValue(L"status", JsonValue::CreateStringValue(L"failed"));
  return responseJson;
}

JsonObject TestCommandResponse::Exception(const ExceptionInfo &err) noexcept {
  JsonObject responseJson;
  responseJson.SetNamedValue(L"status", JsonValue::CreateStringValue(L"exception"));
  responseJson.SetNamedValue(L"message", JsonValue::CreateStringValue(err.Message));
  responseJson.SetNamedValue(L"originalMessage", JsonValue::CreateStringValue(err.OriginalMessage));
  responseJson.SetNamedValue(L"name", JsonValue::CreateStringValue(err.Name));

  JsonArray callstack;

  for (const auto &frame : err.Callstack) {
    JsonObject frameJson;
    frameJson.SetNamedValue(L"file", JsonValue::CreateStringValue(frame.File));
    frameJson.SetNamedValue(L"method", JsonValue::CreateStringValue(frame.Method));
    frameJson.SetNamedValue(L"line", JsonValue::CreateNumberValue(frame.Line));
    frameJson.SetNamedValue(L"column", JsonValue::CreateNumberValue(frame.Column));

    callstack.Append(frameJson);
  }

  responseJson.SetNamedValue(L"callstack", callstack);

  return responseJson;
}

JsonObject TestCommandResponse::Error(std::string_view message) noexcept {
  JsonObject responseJson;
  responseJson.SetNamedValue(L"status", JsonValue::CreateStringValue(L"error"));
  responseJson.SetNamedValue(L"message", JsonValue::CreateStringValue(winrt::to_hstring(message)));
  return responseJson;
}

} // namespace IntegrationTest
