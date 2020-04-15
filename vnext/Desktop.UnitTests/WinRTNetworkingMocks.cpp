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
  IOutputStream MockMessageWebSocket::OutputStream() const
  {
    return {};
  }

  IAsyncAction MockMessageWebSocket::ConnectAsync(Uri const& uri) const
  {
    return {};
  }

  void MockMessageWebSocket::SetRequestHeader(hstring const& headerName, hstring const& headerValue) const {}

  event_token MockMessageWebSocket::Closed(TypedEventHandler<IWebSocket, IWebSocketClosedEventArgs> const& eventHandler) const
  {
    return {};
  }

  IWebSocket::Closed_revoker MockMessageWebSocket::Closed(auto_revoke_t, TypedEventHandler<IWebSocket, IWebSocketClosedEventArgs> const& eventHandler) const
  {
    return {};
  }

  void MockMessageWebSocket::Closed(event_token const& eventCookie) const noexcept {}

  void MockMessageWebSocket::Close(uint16_t code, hstring const& reason) const {}

  // IMessageWebSocket
  MessageWebSocketControl MockMessageWebSocket::Control() const
  {
    return nullptr;
  }

  MessageWebSocketInformation MockMessageWebSocket::Information() const
  {
    return nullptr;
  }

  event_token MockMessageWebSocket::MessageReceived(TypedEventHandler<IMessageWebSocket, IMessageWebSocketMessageReceivedEventArgs> const& eventHandler) const
  {
    return {};
  }

  IMessageWebSocket::MessageReceived_revoker MockMessageWebSocket::MessageReceived(auto_revoke_t, TypedEventHandler<IMessageWebSocket, IMessageWebSocketMessageReceivedEventArgs> const& eventHandler) const
  {
    return {};
  }

  void MockMessageWebSocket::MessageReceived(event_token const& eventCookie) const noexcept {}

#pragma endregion MockMessageWebSocket

} // Microsoft::React::Test
