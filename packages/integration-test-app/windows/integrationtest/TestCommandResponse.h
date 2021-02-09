// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Data.Json.h>

#include "ExceptionInfo.h"

namespace IntegrationTest::TestCommandResponse {

winrt::Windows::Data::Json::JsonObject Okay() noexcept;
winrt::Windows::Data::Json::JsonObject Timeout() noexcept;
winrt::Windows::Data::Json::JsonObject TestPassed() noexcept;
winrt::Windows::Data::Json::JsonObject TestFailed() noexcept;
winrt::Windows::Data::Json::JsonObject Exception(const ExceptionInfo &err) noexcept;
winrt::Windows::Data::Json::JsonObject Error(std::string_view message) noexcept;

} // namespace IntegrationTest::TestCommandResponse
