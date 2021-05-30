// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Networking.Sockets.h>
#include <functional>

namespace Microsoft::React::Test {

/// <summary>
//  Mocks winrt::Windows::Networking::Sockets::MessageWebSocket.
//  Behavior can be mocked per each interface mehtod.
/// </summary>
struct MockMessageWebSocket : public winrt::implements<
                                  MockMessageWebSocket,
                                  winrt::Windows::Networking::Sockets::IMessageWebSocket,
                                  winrt::Windows::Networking::Sockets::IWebSocket> {
  MockMessageWebSocket();

  struct Mocks {
    // IWebSocket
    std::function<winrt::Windows::Foundation::IAsyncAction(winrt::Windows::Foundation::Uri const &) /*const*/>
        ConnectAsync;

    std::function<void(winrt::param::hstring const &, winrt::param::hstring const &) /*const*/> SetRequestHeader;

    std::function<winrt::Windows::Storage::Streams::IOutputStream() /*const*/> OutputStream;

    std::function<void(std::uint16_t, winrt::param::hstring const &) /*const*/> Close;

    std::function<winrt::event_token(winrt::Windows::Foundation::TypedEventHandler<
                                     winrt::Windows::Networking::Sockets::IWebSocket,
                                     winrt::Windows::Networking::Sockets::WebSocketClosedEventArgs> const &) /*const*/>
        ClosedToken;

    std::function<winrt::Windows::Networking::Sockets::IMessageWebSocket::Closed_revoker(
        winrt::auto_revoke_t,
        winrt::Windows::Foundation::TypedEventHandler<
            winrt::Windows::Networking::Sockets::IWebSocket,
            winrt::Windows::Networking::Sockets::WebSocketClosedEventArgs> const &) /*const*/>
        ClosedRevoker;

    std::function<void(winrt::event_token const &) /*const*/ /*noexcept*/> ClosedVoid;

    // IMessageWebSocket
    std::function<winrt::Windows::Networking::Sockets::MessageWebSocketControl() /*const*/> Control;

    std::function<winrt::Windows::Networking::Sockets::MessageWebSocketInformation() /*const*/> Information;

    std::function<winrt::event_token(
        winrt::Windows::Foundation::TypedEventHandler<
            winrt::Windows::Networking::Sockets::MessageWebSocket,
            winrt::Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs> const &) /*const*/>
        MessageReceivedToken;

    std::function<winrt::Windows::Networking::Sockets::IMessageWebSocket::MessageReceived_revoker(
        winrt::auto_revoke_t,
        winrt::Windows::Foundation::TypedEventHandler<
            winrt::Windows::Networking::Sockets::MessageWebSocket,
            winrt::Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs> const &) /*const*/>
        MessageReceivedRevoker;

    std::function<void(winrt::event_token const &)> MessageReceivedVoid;
  };

  Mocks Mocks;

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
                            winrt::Windows::Networking::Sockets::WebSocketClosedEventArgs> const &eventHandler) const;

  winrt::Windows::Networking::Sockets::IMessageWebSocket::Closed_revoker Closed(
      winrt::auto_revoke_t,
      winrt::Windows::Foundation::TypedEventHandler<
          winrt::Windows::Networking::Sockets::IWebSocket,
          winrt::Windows::Networking::Sockets::WebSocketClosedEventArgs> const &eventHandler) const;

  void Closed(winrt::event_token const &eventCookie) const noexcept;

#pragma endregion IWebSocket overrides

#pragma region IMessageWebSocket overrides

  winrt::Windows::Networking::Sockets::MessageWebSocketControl Control() const;
  winrt::Windows::Networking::Sockets::MessageWebSocketInformation Information() const;
  winrt::event_token MessageReceived(
      winrt::Windows::Foundation::TypedEventHandler<
          winrt::Windows::Networking::Sockets::MessageWebSocket,
          winrt::Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs> const &eventHandler) const;
  winrt::Windows::Networking::Sockets::IMessageWebSocket::MessageReceived_revoker MessageReceived(
      winrt::auto_revoke_t,
      winrt::Windows::Foundation::TypedEventHandler<
          winrt::Windows::Networking::Sockets::MessageWebSocket,
          winrt::Windows::Networking::Sockets::MessageWebSocketMessageReceivedEventArgs> const &eventHandler) const;
  void MessageReceived(winrt::event_token const &eventCookie) const noexcept;

#pragma endregion IMessageWebSocket overrides

 private:
  // Used only for MessageWebSocketControl.
  // Remove once MockMessageWebSocketControl is fully implemented.
  winrt::Windows::Networking::Sockets::MessageWebSocket m_messageWebSocket;

}; // MockMessageWebSocket

struct ThrowingMessageWebSocket : public MockMessageWebSocket {
  ThrowingMessageWebSocket();
};

struct MockDataWriter : public winrt::Windows::Storage::Streams::IDataWriter {
  struct Mocks {
    std::function<std::uint32_t() /*const*/> UnstoredBufferLength;
    std::function<winrt::Windows::Storage::Streams::UnicodeEncoding() /*const*/> GetUnicodeEncoding;
    std::function<void(winrt::Windows::Storage::Streams::UnicodeEncoding const &value) /*const*/> SetUnicodeEncoding;
    std::function<winrt::Windows::Storage::Streams::ByteOrder() /*const*/> GetByteOrder;
    std::function<void(winrt::Windows::Storage::Streams::ByteOrder const &value) /*const*/> SetByteOrder;
    std::function<void(std::uint8_t value) /*const*/> WriteByte;
    std::function<void(winrt::array_view<std::uint8_t const> value) /*const*/> WriteBytes;
    std::function<void(winrt::Windows::Storage::Streams::IBuffer const &buffer) /*const*/> WriteBuffer;
    std::function<void(
        winrt::Windows::Storage::Streams::IBuffer const &buffer,
        std::uint32_t start,
        std::uint32_t count) /*const*/>
        WriteBufferRange;
    std::function<void(bool value) /*const*/> WriteBoolean;
    std::function<void(winrt::guid const &value) /*const*/> WriteGuid;
    std::function<void(std::int16_t value) /*const*/> WriteInt16;
    std::function<void(std::int32_t value) /*const*/> WriteInt32;
    std::function<void(std::int64_t value) /*const*/> WriteInt64;
    std::function<void(std::uint16_t value) /*const*/> WriteUInt16;
    std::function<void(std::uint32_t value) /*const*/> WriteUInt32;
    std::function<void(std::uint64_t value) /*const*/> WriteUInt64;
    std::function<void(float value) /*const*/> WriteSingle;
    std::function<void(double value) /*const*/> WriteDouble;
    std::function<void(winrt::Windows::Foundation::DateTime const &value) /*const*/> WriteDateTime;
    std::function<void(winrt::Windows::Foundation::TimeSpan const &value) /*const*/> WriteTimeSpan;
    std::function<std::uint32_t(winrt::param::hstring const &value) /*const*/> WriteString;
    std::function<std::uint32_t(winrt::param::hstring const &value) /*const*/> MeasureString;
    std::function<winrt::Windows::Storage::Streams::DataWriterStoreOperation() /*const*/> StoreAsync;
    std::function<winrt::Windows::Foundation::IAsyncOperation<bool>() /*const*/> FlushAsync;
    std::function<winrt::Windows::Storage::Streams::IBuffer() /*const*/> DetachBuffer;
    std::function<winrt::Windows::Storage::Streams::IOutputStream() /*const*/> DetachStream;
  };

  Mocks Mocks;

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

struct MockMessageWebSocketControl : winrt::implements<
                                         MockMessageWebSocketControl,
                                         winrt::Windows::Networking::Sockets::IMessageWebSocketControl,
                                         winrt::Windows::Networking::Sockets::IWebSocketControl> {
#pragma region IWebSocketControl

  std::uint32_t OutboundBufferSizeInBytes() const;
  void OutboundBufferSizeInBytes(std::uint32_t value) const;
  winrt::Windows::Security::Credentials::PasswordCredential ServerCredential() const;
  void ServerCredential(winrt::Windows::Security::Credentials::PasswordCredential const &value) const;
  winrt::Windows::Security::Credentials::PasswordCredential ProxyCredential() const;
  void ProxyCredential(winrt::Windows::Security::Credentials::PasswordCredential const &value) const;
  winrt::Windows::Foundation::Collections::IVector<winrt::hstring> SupportedProtocols() const;

#pragma endregion

#pragma region IMessageWebSocketControl

  std::uint32_t MaxMessageSize() const;
  void MaxMessageSize(std::uint32_t value) const;
  winrt::Windows::Networking::Sockets::SocketMessageType MessageType() const;
  void MessageType(winrt::Windows::Networking::Sockets::SocketMessageType const &value) const;

#pragma endregion
};

} // namespace Microsoft::React::Test
