// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "WinRTNetworkingMocks.h"

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Networking::Sockets;

using winrt::auto_revoke_t;
using winrt::event_token;
using winrt::param::hstring;
using winrt::Windows::Storage::Streams::IOutputStream;

namespace Microsoft::React::Test {

#pragma region MockMessageWebSocket

// IWebSocket
IOutputStream MockMessageWebSocket::OutputStream() const {
  if (OutputStreamMock)
    return OutputStreamMock();

  throw std::exception("Not implemented");
}

IAsyncAction MockMessageWebSocket::ConnectAsync(Uri const &uri) const {
  if (ConnectAsyncMock)
    return ConnectAsyncMock(uri);

  throw std::exception("Not implemented");
}

void MockMessageWebSocket::SetRequestHeader(hstring const &headerName, hstring const &headerValue) const {
  if (SetRequestHeaderMock)
    SetRequestHeaderMock(headerName, headerValue);

  throw std::exception("Not implemented");
}

event_token MockMessageWebSocket::Closed(
    TypedEventHandler<IWebSocket, IWebSocketClosedEventArgs> const &eventHandler) const {
  if (ClosedTokenMock)
    return ClosedTokenMock(eventHandler);

  throw std::exception("Not implemented");
}

IWebSocket::Closed_revoker MockMessageWebSocket::Closed(
    auto_revoke_t autoRevoke,
    TypedEventHandler<IWebSocket, IWebSocketClosedEventArgs> const &eventHandler) const {
  if (ClosedRevokerMock)
    return ClosedRevokerMock(autoRevoke, eventHandler);

  throw std::exception("Not implemented");
}

void MockMessageWebSocket::Closed(event_token const &eventCookie) const noexcept {
  if (ClosedVoidMock)
    ClosedVoidMock(eventCookie);
}

void MockMessageWebSocket::Close(uint16_t code, hstring const &reason) const {
  if (CloseMock)
    CloseMock(code, reason);

  throw std::exception("Not implemented");
}

// IMessageWebSocket
MessageWebSocketControl MockMessageWebSocket::Control() const {
  if (ControlMock)
    return ControlMock();

  throw std::exception("Not implemented");
}

MessageWebSocketInformation MockMessageWebSocket::Information() const {
  if (InformationMock)
    InformationMock();

  throw std::exception("Not implemented");
}

event_token MockMessageWebSocket::MessageReceived(
    TypedEventHandler<IMessageWebSocket, IMessageWebSocketMessageReceivedEventArgs> const &eventHandler) const {
  if (MessageReceivedTokenMock)
    return MessageReceivedTokenMock(eventHandler);

  throw std::exception("Not implemented");
}

IMessageWebSocket::MessageReceived_revoker MockMessageWebSocket::MessageReceived(
    auto_revoke_t autoRevoke,
    TypedEventHandler<IMessageWebSocket, IMessageWebSocketMessageReceivedEventArgs> const &eventHandler) const {
  if (MessageReceivedRevokerMock)
    return MessageReceivedRevokerMock(autoRevoke, eventHandler);

  throw std::exception("Not implemented");
}

void MockMessageWebSocket::MessageReceived(event_token const &eventCookie) const noexcept {
  if (MessageReceivedVoidMock)
    MessageReceivedVoidMock(eventCookie);
}

#pragma endregion MockMessageWebSocket

} // namespace Microsoft::React::Test
