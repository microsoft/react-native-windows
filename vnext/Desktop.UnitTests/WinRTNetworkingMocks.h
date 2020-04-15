// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Networking.Sockets.h>
#include <functional>

namespace Microsoft::React::Test {

struct MockMessageWebSocket : public winrt::Windows::Networking::Sockets::IMessageWebSocket {
#pragma region Mocks

  // IWebSocket
  std::function<winrt::Windows::Foundation::IAsyncAction(winrt::Windows::Foundation::Uri const &) /*const*/>
      ConnectAsyncMock;

  std::function<void(winrt::param::hstring const &, winrt::param::hstring const &) /*const*/> SetRequestHeaderMock;

  std::function<winrt::Windows::Storage::Streams::IOutputStream() /*const*/> OutputStreamMock;

  std::function<void(std::uint16_t, winrt::param::hstring const &) /*const*/> CloseMock;

  std::function<winrt::event_token(winrt::Windows::Foundation::TypedEventHandler<
                                   winrt::Windows::Networking::Sockets::IWebSocket,
                                   winrt::Windows::Networking::Sockets::IWebSocketClosedEventArgs> const &) /*const*/>
      ClosedTokenMock;

  std::function<Closed_revoker(
      winrt::auto_revoke_t,
      winrt::Windows::Foundation::TypedEventHandler<
          winrt::Windows::Networking::Sockets::IWebSocket,
          winrt::Windows::Networking::Sockets::IWebSocketClosedEventArgs> const &) /*const*/>
      ClosedRevokerMock;

  std::function<void(winrt::event_token const &) /*const*/ /*noexcept*/> ClosedVoidMock;

  // IMessageWebSocket
  std::function<winrt::Windows::Networking::Sockets::MessageWebSocketControl() /*const*/> ControlMock;

  std::function<winrt::Windows::Networking::Sockets::MessageWebSocketInformation() /*const*/> InformationMock;

  std::function<winrt::event_token(
      winrt::Windows::Foundation::TypedEventHandler<
          winrt::Windows::Networking::Sockets::IMessageWebSocket,
          winrt::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs> const &) /*const*/>
      MessageReceivedTokenMock;

  std::function<MessageReceived_revoker(
      winrt::auto_revoke_t,
      winrt::Windows::Foundation::TypedEventHandler<
          winrt::Windows::Networking::Sockets::IMessageWebSocket,
          winrt::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs> const &) /*const*/>
      MessageReceivedRevokerMock;

  std::function<void(winrt::event_token const &)> MessageReceivedVoidMock;

#pragma endregion

#pragma region IWebSocket overrides

  // Methods

  winrt::Windows::Foundation::IAsyncAction ConnectAsync(winrt::Windows::Foundation::Uri const &uri) const;

  void SetRequestHeader(winrt::param::hstring const &headerName, winrt::param::hstring const &headerValue) const;

  winrt::Windows::Storage::Streams::IOutputStream OutputStream() const;

  void Close(std::uint16_t code, winrt::param::hstring const &reason) const;

  // Events

  // TODO: WebSocketClosedEventArgs instead?
  winrt::event_token Closed(winrt::Windows::Foundation::TypedEventHandler<
                            winrt::Windows::Networking::Sockets::IWebSocket,
                            winrt::Windows::Networking::Sockets::IWebSocketClosedEventArgs> const &eventHandler) const;

  Closed_revoker Closed(
      winrt::auto_revoke_t,
      winrt::Windows::Foundation::TypedEventHandler<
          winrt::Windows::Networking::Sockets::IWebSocket,
          winrt::Windows::Networking::Sockets::IWebSocketClosedEventArgs> const &eventHandler) const;

  void Closed(winrt::event_token const &eventCookie) const noexcept;

#pragma endregion IWebSocket overrides

#pragma region IMessageWebSocket overrides

  // Methods

  // TODO: IMessageWebSocketControl?
  winrt::Windows::Networking::Sockets::MessageWebSocketControl Control() const;

  winrt::Windows::Networking::Sockets::MessageWebSocketInformation Information() const;

  // Events

  winrt::event_token MessageReceived(
      winrt::Windows::Foundation::TypedEventHandler<
          winrt::Windows::Networking::Sockets::IMessageWebSocket,
          winrt::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs> const &eventHandler) const;

  MessageReceived_revoker MessageReceived(
      winrt::auto_revoke_t,
      winrt::Windows::Foundation::TypedEventHandler<
          winrt::Windows::Networking::Sockets::IMessageWebSocket,
          winrt::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs> const &eventHandler) const;

  void MessageReceived(winrt::event_token const &eventCookie) const noexcept;

#pragma endregion IMessageWebSocket overrides

}; // MockMessageWebSocket

struct MockDataWriter : public winrt::Windows::Storage::Streams::IDataWriter {
#pragma region Mocks

  std::function<std::uint32_t() /*const*/> UnstoredBufferLengthMock;
  std::function<winrt::Windows::Storage::Streams::UnicodeEncoding() /*const*/> GetUnicodeEncodingMock;
  std::function<void(winrt::Windows::Storage::Streams::UnicodeEncoding const &value) /*const*/> SetUnicodeEncodingMock;
  std::function<winrt::Windows::Storage::Streams::ByteOrder() /*const*/> GetByteOrderMock;
  std::function<void(winrt::Windows::Storage::Streams::ByteOrder const &value) /*const*/> SetByteOrderMock;
  std::function<void(std::uint8_t value) /*const*/> WriteByteMock;
  std::function<void(winrt::array_view<std::uint8_t const> value) /*const*/> WriteBytesMock;
  std::function<void(winrt::Windows::Storage::Streams::IBuffer const &buffer) /*const*/> WriteBufferMock;
  std::function<
      void(winrt::Windows::Storage::Streams::IBuffer const &buffer, std::uint32_t start, std::uint32_t count) /*const*/>
      WriteBufferRangeMock;
  std::function<void(bool value) /*const*/> WriteBooleanMock;
  std::function<void(winrt::guid const &value) /*const*/> WriteGuidMock;
  std::function<void(std::int16_t value) /*const*/> WriteInt16Mock;
  std::function<void(std::int32_t value) /*const*/> WriteInt32Mock;
  std::function<void(std::int64_t value) /*const*/> WriteInt64Mock;
  std::function<void(std::uint16_t value) /*const*/> WriteUInt16Mock;
  std::function<void(std::uint32_t value) /*const*/> WriteUInt32Mock;
  std::function<void(std::uint64_t value) /*const*/> WriteUInt64Mock;
  std::function<void(float value) /*const*/> WriteSingleMock;
  std::function<void(double value) /*const*/> WriteDoubleMock;
  std::function<void(winrt::Windows::Foundation::DateTime const &value) /*const*/> WriteDateTimeMock;
  std::function<void(winrt::Windows::Foundation::TimeSpan const &value) /*const*/> WriteTimeSpanMock;
  std::function<std::uint32_t(winrt::param::hstring const &value) /*const*/> WriteStringMock;
  std::function<std::uint32_t(winrt::param::hstring const &value) /*const*/> MeasureStringMock;
  std::function<winrt::Windows::Storage::Streams::DataWriterStoreOperation() /*const*/> StoreAsyncMock;
  std::function<winrt::Windows::Foundation::IAsyncOperation<bool>() /*const*/> FlushAsyncMock;
  std::function<winrt::Windows::Storage::Streams::IBuffer() /*const*/> DetachBufferMock;
  std::function<winrt::Windows::Storage::Streams::IOutputStream() /*const*/> DetachStreamMock;

#pragma endregion Mocks

#pragma region IDataWriter overrides

  std::uint32_t UnstoredBufferLength() const;
  winrt::Windows::Storage::Streams::UnicodeEncoding UnicodeEncoding() const;
  void UnicodeEncoding(winrt::Windows::Storage::Streams::UnicodeEncoding const &value) const;
  winrt::Windows::Storage::Streams::ByteOrder ByteOrder() const;
  void ByteOrder(winrt::Windows::Storage::Streams::ByteOrder const &value) const;
  void WriteByte(std::uint8_t value) const;
  void WriteBytes(winrt::array_view<std::uint8_t const> value) const;
  void WriteBuffer(winrt::Windows::Storage::Streams::IBuffer const &buffer) const;
  void WriteBuffer(winrt::Windows::Storage::Streams::IBuffer const &buffer, std::uint32_t start, std::uint32_t count)
      const;
  void WriteBoolean(bool value) const;
  void WriteGuid(winrt::guid const &value) const;
  void WriteInt16(std::int16_t value) const;
  void WriteInt32(std::int32_t value) const;
  void WriteInt64(std::int64_t value) const;
  void WriteUInt16(std::uint16_t value) const;
  void WriteUInt32(std::uint32_t value) const;
  void WriteUInt64(std::uint64_t value) const;
  void WriteSingle(float value) const;
  void WriteDouble(double value) const;
  void WriteDateTime(winrt::Windows::Foundation::DateTime const &value) const;
  void WriteTimeSpan(winrt::Windows::Foundation::TimeSpan const &value) const;
  std::uint32_t WriteString(winrt::param::hstring const &value) const;
  std::uint32_t MeasureString(winrt::param::hstring const &value) const;
  winrt::Windows::Storage::Streams::DataWriterStoreOperation StoreAsync() const;
  winrt::Windows::Foundation::IAsyncOperation<bool> FlushAsync() const;
  winrt::Windows::Storage::Streams::IBuffer DetachBuffer() const;
  winrt::Windows::Storage::Streams::IOutputStream DetachStream() const;

#pragma endregion IDataWriter overrides

}; // MockDataWriter

} // namespace Microsoft::React::Test
