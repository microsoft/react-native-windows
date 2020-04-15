// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Networking.Sockets.h>

namespace Microsoft::React::Test {

struct MockMessageWebSocket : public winrt::Windows::Networking::Sockets::IMessageWebSocket
{

#pragma region IWebSocket overrides

  // Methods

  winrt::Windows::Foundation::IAsyncAction ConnectAsync(winrt::Windows::Foundation::Uri const& uri) const;

  void SetRequestHeader(winrt::param::hstring const& headerName, winrt::param::hstring const& headerValue) const;

  winrt::Windows::Storage::Streams::IOutputStream OutputStream() const;

  void Close(std::uint16_t code, winrt::param::hstring const& reason) const;

  // Events

  //TODO: WebSocketClosedEventArgs instead?
  winrt::event_token Closed(winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Networking::Sockets::IWebSocket, winrt::Windows::Networking::Sockets::IWebSocketClosedEventArgs> const& eventHandler) const;

  Closed_revoker Closed(winrt::auto_revoke_t, winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Networking::Sockets::IWebSocket, winrt::Windows::Networking::Sockets::IWebSocketClosedEventArgs> const& eventHandler) const;

  void Closed(winrt::event_token const& eventCookie) const noexcept;

#pragma endregion IWebSocket overrides

#pragma region IMessageWebSocket overrides

  // Methods

  //TODO: IMessageWebSocketControl?
  winrt::Windows::Networking::Sockets::MessageWebSocketControl Control() const;

  winrt::Windows::Networking::Sockets::MessageWebSocketInformation Information() const;

  // Events

  winrt::event_token MessageReceived(winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Networking::Sockets::IMessageWebSocket, winrt::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs> const& eventHandler) const;

  MessageReceived_revoker MessageReceived(winrt::auto_revoke_t, winrt::Windows::Foundation::TypedEventHandler<winrt::Windows::Networking::Sockets::IMessageWebSocket, winrt::Windows::Networking::Sockets::IMessageWebSocketMessageReceivedEventArgs> const& eventHandler) const;

  void MessageReceived(winrt::event_token const& eventCookie) const noexcept;

#pragma endregion IMessageWebSocket overrides

}; // MockMessageWebSocket

} // Microsoft::React::Test
