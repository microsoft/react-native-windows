// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "WinRTNetworkingMocks.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Sockets;
using namespace winrt::Windows::Storage::Streams;

using std::exception;
using winrt::auto_revoke_t;
using winrt::event_token;
using winrt::param::hstring;

namespace Microsoft::React::Test {

#pragma region MockMessageWebSocket

// IWebSocket
IOutputStream MockMessageWebSocket::OutputStream() const {
  if (OutputStreamMock)
    return OutputStreamMock();

  throw exception("Not implemented");
}

IAsyncAction MockMessageWebSocket::ConnectAsync(Uri const &uri) const {
  if (ConnectAsyncMock)
    return ConnectAsyncMock(uri);

  throw exception("Not implemented");
}

void MockMessageWebSocket::SetRequestHeader(hstring const &headerName, hstring const &headerValue) const {
  if (SetRequestHeaderMock)
    return SetRequestHeaderMock(headerName, headerValue);

  throw exception("Not implemented");
}

event_token MockMessageWebSocket::Closed(
    TypedEventHandler<IWebSocket, IWebSocketClosedEventArgs> const &eventHandler) const {
  if (ClosedTokenMock)
    return ClosedTokenMock(eventHandler);

  throw exception("Not implemented");
}

IWebSocket::Closed_revoker MockMessageWebSocket::Closed(
    auto_revoke_t autoRevoke,
    TypedEventHandler<IWebSocket, IWebSocketClosedEventArgs> const &eventHandler) const {
  if (ClosedRevokerMock)
    return ClosedRevokerMock(autoRevoke, eventHandler);

  throw exception("Not implemented");
}

void MockMessageWebSocket::Closed(event_token const &eventCookie) const noexcept {
  if (ClosedVoidMock)
    ClosedVoidMock(eventCookie);
}

void MockMessageWebSocket::Close(uint16_t code, hstring const &reason) const {
  if (CloseMock)
    return CloseMock(code, reason);

  throw exception("Not implemented");
}

// IMessageWebSocket
MessageWebSocketControl MockMessageWebSocket::Control() const {
  if (ControlMock)
    return ControlMock();

  throw exception("Not implemented");
}

MessageWebSocketInformation MockMessageWebSocket::Information() const {
  if (InformationMock)
    return InformationMock();

  throw exception("Not implemented");
}

event_token MockMessageWebSocket::MessageReceived(
    TypedEventHandler<IMessageWebSocket, IMessageWebSocketMessageReceivedEventArgs> const &eventHandler) const {
  if (MessageReceivedTokenMock)
    return MessageReceivedTokenMock(eventHandler);

  throw exception("Not implemented");
}

IMessageWebSocket::MessageReceived_revoker MockMessageWebSocket::MessageReceived(
    auto_revoke_t autoRevoke,
    TypedEventHandler<IMessageWebSocket, IMessageWebSocketMessageReceivedEventArgs> const &eventHandler) const {
  if (MessageReceivedRevokerMock)
    return MessageReceivedRevokerMock(autoRevoke, eventHandler);

  throw exception("Not implemented");
}

void MockMessageWebSocket::MessageReceived(event_token const &eventCookie) const noexcept {
  if (MessageReceivedVoidMock)
    MessageReceivedVoidMock(eventCookie);
}

#pragma endregion MockMessageWebSocket

#pragma region MockDataWriter

uint32_t MockDataWriter::UnstoredBufferLength() const {
  if (UnstoredBufferLengthMock)
    return UnstoredBufferLengthMock();

  throw exception("Not implemented");
}

UnicodeEncoding MockDataWriter::UnicodeEncoding() const {
  if (GetUnicodeEncodingMock)
    return GetUnicodeEncodingMock();

  throw exception("Not implemented");
}

void MockDataWriter::UnicodeEncoding(winrt::Windows::Storage::Streams::UnicodeEncoding const &value) const {
  if (SetUnicodeEncodingMock)
    return SetUnicodeEncodingMock(value);

  throw exception("Not implemented");
}

ByteOrder MockDataWriter::ByteOrder() const {
  if (GetByteOrderMock)
    return GetByteOrderMock();

  throw exception("Not implemented");
}

void MockDataWriter::ByteOrder(winrt::Windows::Storage::Streams::ByteOrder const &value) const {
  if (SetByteOrderMock)
    return SetByteOrderMock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteByte(uint8_t value) const {
  if (WriteByteMock)
    return WriteByteMock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteBytes(winrt::array_view<uint8_t const> value) const {
  if (WriteBytesMock)
    return WriteBytesMock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteBuffer(IBuffer const &buffer) const {
  if (WriteBufferMock)
    return WriteBufferMock(buffer);

  throw exception("Not implemented");
}

void MockDataWriter::WriteBuffer(IBuffer const &buffer, uint32_t start, uint32_t count) const {
  if (WriteBufferRangeMock)
    return WriteBufferRangeMock(buffer, start, count);

  throw exception("Not implemented");
}

void MockDataWriter::WriteBoolean(bool value) const {
  if (WriteBooleanMock)
    return WriteBooleanMock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteGuid(winrt::guid const &value) const {
  if (WriteGuidMock)
    return WriteGuidMock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteInt16(int16_t value) const {
  if (WriteInt16Mock)
    return WriteInt16Mock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteInt32(int32_t value) const {
  if (WriteInt32Mock)
    return WriteInt32Mock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteInt64(int64_t value) const {
  if (WriteInt64Mock)
    return WriteInt64Mock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteUInt16(uint16_t value) const {
  if (WriteUInt16Mock)
    return WriteUInt16Mock(value);
}

void MockDataWriter::WriteUInt32(uint32_t value) const {
  if (WriteUInt32Mock)
    return WriteUInt32Mock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteUInt64(uint64_t value) const {
  if (WriteUInt64Mock)
    return WriteUInt64Mock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteSingle(float value) const {
  if (WriteSingleMock)
    return WriteSingleMock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteDouble(double value) const {
  if (WriteDoubleMock)
    return WriteDoubleMock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteDateTime(DateTime const &value) const {
  if (WriteDateTimeMock)
    return WriteDateTimeMock(value);

  throw exception("Not implemented");
}

void MockDataWriter::WriteTimeSpan(TimeSpan const &value) const {
  if (WriteTimeSpanMock)
    return WriteTimeSpanMock(value);

  throw exception("Not implemented");
}

uint32_t MockDataWriter::WriteString(hstring const &value) const {
  if (WriteStringMock)
    return WriteStringMock(value);

  throw exception("Not implemented");
}

uint32_t MockDataWriter::MeasureString(hstring const &value) const {
  if (MeasureStringMock)
    return MeasureStringMock(value);

  throw exception("Not implemented");
}

DataWriterStoreOperation MockDataWriter::StoreAsync() const {
  if (StoreAsyncMock)
    return StoreAsyncMock();

  throw exception("Not implemented");
}

IAsyncOperation<bool> MockDataWriter::FlushAsync() const {
  if (FlushAsyncMock)
    return FlushAsyncMock();

  throw exception("Not implemented");
}

IBuffer MockDataWriter::DetachBuffer() const {
  if (DetachBufferMock)
    return DetachBufferMock();

  throw exception("Not implemented");
}

IOutputStream MockDataWriter::DetachStream() const {
  if (DetachStreamMock)
    return DetachStreamMock();

  throw exception("Not implemented");
}

#pragma endregion MockDataWriter

} // namespace Microsoft::React::Test
