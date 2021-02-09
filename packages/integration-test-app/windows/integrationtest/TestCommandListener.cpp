// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TestCommandListener.h"

#include <winrt/Windows.Storage.Streams.h>

using namespace winrt::integrationtest;
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Data::Json;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Storage::Streams;

namespace IntegrationTest {

IAsyncAction TestCommandResponse::Okay() noexcept {
  JsonObject responseJson;
  responseJson.SetNamedValue(L"status", JsonValue::CreateStringValue(L"okay"));
  co_await SendJson(responseJson);
}

IAsyncAction TestCommandResponse::Timeout() noexcept {
  JsonObject responseJson;
  responseJson.SetNamedValue(L"status", JsonValue::CreateStringValue(L"timeout"));
  co_await SendJson(responseJson);
}

IAsyncAction TestCommandResponse::TestPassed() noexcept {
  JsonObject responseJson;
  responseJson.SetNamedValue(L"status", JsonValue::CreateStringValue(L"passed"));
  co_await SendJson(responseJson);
}

IAsyncAction TestCommandResponse::TestFailed() noexcept {
  JsonObject responseJson;
  responseJson.SetNamedValue(L"status", JsonValue::CreateStringValue(L"failed"));
  co_await SendJson(responseJson);
}

IAsyncAction TestCommandResponse::Exception(const ExceptionInfo &err) noexcept {
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

  co_await SendJson(responseJson);
}

IAsyncAction TestCommandResponse::Error(std::string_view message) noexcept {
  JsonObject responseJson;
  responseJson.SetNamedValue(L"status", JsonValue::CreateStringValue(L"error"));
  responseJson.SetNamedValue(L"message", JsonValue::CreateStringValue(winrt::to_hstring(message)));
  co_await SendJson(responseJson);
}

IAsyncAction TestCommandResponse::SendJson(const JsonObject &payload) noexcept {
  winrt::hstring utf16Str = payload.ToString();
  std::string utf8Str = winrt::to_string(utf16Str);
  winrt::array_view<byte> utf8Bytes(
      reinterpret_cast<byte *>(utf8Str.data()), reinterpret_cast<byte *>(utf8Str.data() + utf8Str.size()));

  DataWriter streamWriter(m_socket.OutputStream());

  try {
    streamWriter.ByteOrder(ByteOrder::LittleEndian);
    streamWriter.WriteUInt32(static_cast<uint32_t>(utf8Str.size()));
    streamWriter.WriteBytes(utf8Bytes);

    co_await streamWriter.StoreAsync();
    co_await streamWriter.FlushAsync();

  } catch (const winrt::hresult_error &ex) {
    auto status = SocketError::GetStatus(ex.code());

    // We don't care if something happens to the socket while we're responding.
    // Only throw if there was a non-socket error.
    if (status == SocketErrorStatus::Unknown) {
      throw ex;
    }
  }

  // The DataWriter being destroyed will close the underlying socket stream.
  // Detach it before exiting.
  streamWriter.DetachStream();
}

IAsyncOperation<ListenResult> TestCommandListener::StartListening(int32_t port) noexcept {
  auto weakThis = get_weak();

  try {
    co_await m_socketServer.BindEndpointAsync(HostName(L"127.0.0.1"), winrt::to_hstring(port));

    if (auto strongThis = weakThis.get()) {
      m_connectionReceivedRevoker = m_socketServer.ConnectionReceived(
          winrt::auto_revoke, [this](const auto & /*sender*/, const auto &args) noexcept {
            if (m_socketListenAction && m_socketListenAction.Status() == AsyncStatus::Started) {
              TestCommandResponse(args.Socket()).Error("Connection already in progress");
              return;
            }

            m_socketListenAction = ListenToSocket(args.Socket());
          });

      m_isListening = true;
    }

    co_return ListenResult::Success;
  } catch (const winrt::hresult_error ex) {
    auto status = SocketError::GetStatus(ex.code());
    if (status == SocketErrorStatus::AddressAlreadyInUse) {
      co_return ListenResult::AddressInUse;
    } else {
      co_return ListenResult::OtherError;
    }
  }
}

bool TestCommandListener::IsListening() const noexcept {
  return m_isListening;
}

IAsyncAction TestCommandListener::ListenToSocket(StreamSocket socket) noexcept {
  auto weakThis = get_weak();

  DataReader socketReader(socket.InputStream());
  socketReader.UnicodeEncoding(UnicodeEncoding::Utf8);
  socketReader.ByteOrder(ByteOrder::LittleEndian);

  try {
    while (true) {
      auto loadSizeBytesRead = co_await socketReader.LoadAsync(4 /*bytes*/);
      if (loadSizeBytesRead != 4) {
        co_return;
      }

      auto messageSize = socketReader.ReadUInt32();
      auto loadMessageBytesRead = co_await socketReader.LoadAsync(messageSize);
      if (loadMessageBytesRead != messageSize) {
        co_return;
      }

      auto messageStr = socketReader.ReadString(messageSize);
      auto messageObject = JsonObject::Parse(messageStr);

      if (auto strongThis = weakThis.get()) {
        strongThis->DispatchTestCommand(socket, messageObject);
      }
    }
  } catch (const winrt::hresult_error &ex) {
    auto status = SocketError::GetStatus(ex.code());

    // Allow disconnects
    if (status == SocketErrorStatus::ConnectionResetByPeer) {
      co_return;
    } else {
      throw ex;
    }
  }
}

void TestCommandListener::DispatchTestCommand(
    const winrt::Windows::Networking::Sockets::StreamSocket &socket,
    const JsonObject message) noexcept {
  auto commandId = message.GetNamedString(L"command");
  if (commandId == L"RunTestComponent") {
    auto payload = message.GetNamedValue(L"component");
    m_testCommandEvent(TestCommand{TestCommandId::RunTestComponent, payload}, TestCommandResponse(socket));
  } else if (commandId == L"GoToComponent") {
    auto payload = message.GetNamedValue(L"component");
    m_testCommandEvent(TestCommand{TestCommandId::GoToComponent, payload}, TestCommandResponse(socket));
  } else {
    // Unimplemented
    std::terminate();
  }
}

winrt::event_token TestCommandListener::OnTestCommand(TestCommandDelegate &&delegate) noexcept {
  return m_testCommandEvent.add(std::move(delegate));
}

} // namespace IntegrationTest
