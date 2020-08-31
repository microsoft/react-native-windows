// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "WinRTNetworkingMocks.h"

#include <Windows.h>

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Storage::Streams;

using std::exception;
using winrt::auto_revoke_t;
using winrt::event_token;
using winrt::param::hstring;

namespace Microsoft::React::Test {

#pragma region MockMessageWebSocket

MockMessageWebSocket::MockMessageWebSocket() {
  Mocks.MessageReceivedToken =
      [](TypedEventHandler<MessageWebSocket, MessageWebSocketMessageReceivedEventArgs> const &) -> event_token {
    return event_token{};
  };
}

// IWebSocket
IOutputStream MockMessageWebSocket::OutputStream() const {
  if (Mocks.OutputStream)
    return Mocks.OutputStream();

  throw exception("Not implemented");
}

IAsyncAction MockMessageWebSocket::ConnectAsync(Uri const &uri) const {
  if (Mocks.ConnectAsync)
    return Mocks.ConnectAsync(uri);

  throw exception("Not implemented");
}

void MockMessageWebSocket::SetRequestHeader(hstring const &headerName, hstring const &headerValue) const {
  if (Mocks.SetRequestHeader)
    return Mocks.SetRequestHeader(headerName, headerValue);

  throw exception("Not implemented");
}

event_token MockMessageWebSocket::Closed(
    TypedEventHandler<IWebSocket, WebSocketClosedEventArgs> const &eventHandler) const {
  if (Mocks.ClosedToken)
    return Mocks.ClosedToken(eventHandler);

  throw exception("Not implemented");
}

IWebSocket::Closed_revoker MockMessageWebSocket::Closed(
    auto_revoke_t autoRevoke,
    TypedEventHandler<IWebSocket, WebSocketClosedEventArgs> const &eventHandler) const {
  if (Mocks.ClosedRevoker)
    return Mocks.ClosedRevoker(autoRevoke, eventHandler);

  throw exception("Not implemented");
}

void MockMessageWebSocket::Closed(event_token const &eventCookie) const noexcept {
  if (Mocks.ClosedVoid)
    Mocks.ClosedVoid(eventCookie);
}

void MockMessageWebSocket::Close(uint16_t code, hstring const &reason) const {
  if (Mocks.Close)
    return Mocks.Close(code, reason);

  throw exception("Not implemented");
}

// IMessageWebSocket
MessageWebSocketControl MockMessageWebSocket::Control() const {
  if (Mocks.Control)
    return Mocks.Control();

  return m_messageWebSocket.Control();
};

MessageWebSocketInformation MockMessageWebSocket::Information() const {
  if (Mocks.Information)
    return Mocks.Information();

  throw exception("Not implemented");
}

event_token MockMessageWebSocket::MessageReceived(
    TypedEventHandler<MessageWebSocket, MessageWebSocketMessageReceivedEventArgs> const &eventHandler) const {
  if (Mocks.MessageReceivedToken)
    return Mocks.MessageReceivedToken(eventHandler);

  throw exception("Not implemented");
}

IMessageWebSocket::MessageReceived_revoker MockMessageWebSocket::MessageReceived(
    winrt::auto_revoke_t autoRevoker,
    TypedEventHandler<MessageWebSocket, MessageWebSocketMessageReceivedEventArgs> const &eventHandler) const {
  if (Mocks.MessageReceivedRevoker)
    return Mocks.MessageReceivedRevoker(autoRevoker, eventHandler);

  throw exception("Not implemented");
}

void MockMessageWebSocket::MessageReceived(event_token const &eventCookie) const noexcept {
  if (Mocks.MessageReceivedVoid)
    Mocks.MessageReceivedVoid(eventCookie);
}

#pragma endregion MockMessageWebSocket

#pragma region ThrowingMessageWebSocket

ThrowingMessageWebSocket::ThrowingMessageWebSocket() : MockMessageWebSocket() {
  throw winrt::hresult_error(winrt::hresult(E_FAIL), L"Failed to instantiate WinRT class.");
}

#pragma endregion ThrowingMessageWebSocket

#pragma region MockDataWriter

uint32_t MockDataWriter::UnstoredBufferLength() const {
  if (Mocks.UnstoredBufferLength)
    return Mocks.UnstoredBufferLength();

  throw exception("Not implemented");
}

UnicodeEncoding MockDataWriter::UnicodeEncoding() const {
  if (Mocks.GetUnicodeEncoding)
    return Mocks.GetUnicodeEncoding();

  throw exception("Not implemented");
}

void MockDataWriter::UnicodeEncoding(winrt::Windows::Storage::Streams::UnicodeEncoding const &value) const {
  if (Mocks.SetUnicodeEncoding)
    return Mocks.SetUnicodeEncoding(value);

  throw exception("Not implemented");
}

ByteOrder MockDataWriter::ByteOrder() const {
  if (Mocks.GetByteOrder)
    return Mocks.GetByteOrder();

  throw exception("Not implemented");
}

void MockDataWriter::ByteOrder(winrt::Windows::Storage::Streams::ByteOrder const &value) const {
  if (Mocks.SetByteOrder)
    return Mocks.SetByteOrder(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteByte(uint8_t value) const {
  if (Mocks.WriteByte)
    return Mocks.WriteByte(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteBytes(winrt::array_view<uint8_t const> value) const {
  if (Mocks.WriteBytes)
    return Mocks.WriteBytes(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteBuffer(IBuffer const &buffer) const {
  if (Mocks.WriteBuffer)
    return Mocks.WriteBuffer(buffer);

  throw exception("Not implemented");
}

void MockDataWriter::WriteBuffer(IBuffer const &buffer, uint32_t start, uint32_t count) const {
  if (Mocks.WriteBufferRange)
    return Mocks.WriteBufferRange(buffer, start, count);

  throw exception("Not implemented");
}

void MockDataWriter::WriteBoolean(bool value) const {
  if (Mocks.WriteBoolean)
    return Mocks.WriteBoolean(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteGuid(winrt::guid const &value) const {
  if (Mocks.WriteGuid)
    return Mocks.WriteGuid(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteInt16(int16_t value) const {
  if (Mocks.WriteInt16)
    return Mocks.WriteInt16(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteInt32(int32_t value) const {
  if (Mocks.WriteInt32)
    return Mocks.WriteInt32(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteInt64(int64_t value) const {
  if (Mocks.WriteInt64)
    return Mocks.WriteInt64(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteUInt16(uint16_t value) const {
  if (Mocks.WriteUInt16)
    return Mocks.WriteUInt16(value);
}

void MockDataWriter::WriteUInt32(uint32_t value) const {
  if (Mocks.WriteUInt32)
    return Mocks.WriteUInt32(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteUInt64(uint64_t value) const {
  if (Mocks.WriteUInt64)
    return Mocks.WriteUInt64(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteSingle(float value) const {
  if (Mocks.WriteSingle)
    return Mocks.WriteSingle(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteDouble(double value) const {
  if (Mocks.WriteDouble)
    return Mocks.WriteDouble(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteDateTime(DateTime const &value) const {
  if (Mocks.WriteDateTime)
    return Mocks.WriteDateTime(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteTimeSpan(TimeSpan const &value) const {
  if (Mocks.WriteTimeSpan)
    return Mocks.WriteTimeSpan(value);

  throw exception("Not implemented");
}

uint32_t MockDataWriter::WriteString(hstring const &value) const {
  if (Mocks.WriteString)
    return Mocks.WriteString(value);

  throw exception("Not implemented");
}

uint32_t MockDataWriter::MeasureString(hstring const &value) const {
  if (Mocks.MeasureString)
    return Mocks.MeasureString(value);

  throw exception("Not implemented");
}

DataWriterStoreOperation MockDataWriter::StoreAsync() const {
  if (Mocks.StoreAsync)
    return Mocks.StoreAsync();

  throw exception("Not implemented");
}

IAsyncOperation<bool> MockDataWriter::FlushAsync() const {
  if (Mocks.FlushAsync)
    return Mocks.FlushAsync();

  throw exception("Not implemented");
}

IBuffer MockDataWriter::DetachBuffer() const {
  if (Mocks.DetachBuffer)
    return Mocks.DetachBuffer();

  throw exception("Not implemented");
}

IOutputStream MockDataWriter::DetachStream() const {
  if (Mocks.DetachStream)
    return Mocks.DetachStream();

  throw exception("Not implemented");
}

#pragma endregion MockDataWriter

// TODO: Work around linker error:
// error LNK2019: unresolved external symbol
// "public: __cdecl winrt::Windows::Security::Credentials::PasswordCredential::PasswordCredential(void)"
// (??0PasswordCredential@Credentials@Security@Windows@winrt@@QEAA@XZ)
// referenced in function
// "public: struct winrt::Windows::Security::Credentials::PasswordCredential
// __cdecl Microsoft::React::Test::MockMessageWebSocketControl::ProxyCredential(void)const "
// (?ProxyCredential@MockMessageWebSocketControl@Test@React@Microsoft@@
//  QEBA?AUPasswordCredential@Credentials@Security@Windows@winrt@@XZ)
#if 0
#pragma region MockMessageWebSocketControl

// IWebSocketControl

uint32_t MockMessageWebSocketControl::OutboundBufferSizeInBytes() const {
  return 0;
}

void MockMessageWebSocketControl::OutboundBufferSizeInBytes(uint32_t value) const {}

winrt::Windows::Security::Credentials::PasswordCredential MockMessageWebSocketControl::ServerCredential() const {
  return {};
}

void MockMessageWebSocketControl::ServerCredential(
    winrt::Windows::Security::Credentials::PasswordCredential const &value) const {}

winrt::Windows::Security::Credentials::PasswordCredential MockMessageWebSocketControl::ProxyCredential() const {
  return winrt::Windows::Security::Credentials::PasswordCredential();
}

void MockMessageWebSocketControl::ProxyCredential(
    winrt::Windows::Security::Credentials::PasswordCredential const &value) const {}

winrt::Windows::Foundation::Collections::IVector<winrt::hstring> MockMessageWebSocketControl::SupportedProtocols()
    const {
  return winrt::single_threaded_vector<winrt::hstring>();
}

// IMessageWebSocketControl

uint32_t MockMessageWebSocketControl::MaxMessageSize() const {
  return 0;
}

void MockMessageWebSocketControl::MaxMessageSize(std::uint32_t value) const {}

SocketMessageType MockMessageWebSocketControl::MessageType() const {
  return SocketMessageType::Utf8;
}

void MockMessageWebSocketControl::MessageType(SocketMessageType const &value) const {}

#pragma endregion MockMessageWebsocketControl

#endif // 0

} // namespace Microsoft::React::Test
