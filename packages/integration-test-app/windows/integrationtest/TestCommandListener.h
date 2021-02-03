// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Data.Json.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Networking.Sockets.h>
#include "winrt/integrationtest.h"

#include "ExceptionInfo.h"

namespace IntegrationTest {
//! Command id from the test runner
enum class TestCommandId {
  RunTestComponent,
  GoToComponent,
};

struct TestCommand {
  TestCommandId Id{};
  winrt::Windows::Data::Json::JsonValue payload;
};

class TestCommandResponse {
 public:
  TestCommandResponse(const winrt::Windows::Networking::Sockets::StreamSocket &socket) noexcept : m_socket(socket) {}

  winrt::Windows::Foundation::IAsyncAction Okay() noexcept;
  winrt::Windows::Foundation::IAsyncAction Timeout() noexcept;
  winrt::Windows::Foundation::IAsyncAction TestPassed() noexcept;
  winrt::Windows::Foundation::IAsyncAction TestFailed() noexcept;
  winrt::Windows::Foundation::IAsyncAction Exception(const ExceptionInfo &err) noexcept;
  winrt::Windows::Foundation::IAsyncAction Error(std::string_view message) noexcept;

 private:
  winrt::Windows::Foundation::IAsyncAction SendJson(const winrt::Windows::Data::Json::JsonObject &payload) noexcept;

  winrt::Windows::Networking::Sockets::StreamSocket m_socket;
};

using TestCommandDelegate = winrt::delegate<TestCommand, TestCommandResponse>;

//! Listens for test commands from a loopback TCP server
class TestCommandListener : public winrt::implements<TestCommandListener, winrt::Windows::Foundation::IInspectable> {
 public:
  winrt::Windows::Foundation::IAsyncOperation<winrt::integrationtest::ListenResult> StartListening(
      int32_t port = 30977) noexcept;

  bool IsListening() const noexcept;

  winrt::event_token OnTestCommand(TestCommandDelegate &&delegate) noexcept;

 private:
  winrt::Windows::Foundation::IAsyncAction ListenToSocket(
      winrt::Windows::Networking::Sockets::StreamSocket socket) noexcept;
  void DispatchTestCommand(
      const winrt::Windows::Networking::Sockets::StreamSocket &socket,
      const winrt::Windows::Data::Json::JsonObject message) noexcept;

  winrt::Windows::Networking::Sockets::StreamSocketListener m_socketServer;
  winrt::Windows::Networking::Sockets::IStreamSocketListener::ConnectionReceived_revoker m_connectionReceivedRevoker;
  winrt::Windows::Foundation::IAsyncAction m_socketListenAction;
  bool m_isListening{false};
  winrt::event<TestCommandDelegate> m_testCommandEvent;
};

} // namespace IntegrationTest
