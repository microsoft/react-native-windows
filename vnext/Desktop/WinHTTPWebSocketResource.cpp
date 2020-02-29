// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"

#include "WinHTTPWebSocketResource.h"

using std::function;
using std::size_t;
using std::string;

namespace Microsoft::React
{
WinHTTPWebSocketResource::WinHTTPWebSocketResource(URL_COMPONENTS url, bool isSecure)
  : m_url{ url }
  , m_openFlags{ 0 }
{
  if (isSecure)
    m_openFlags |= WINHTTP_FLAG_SECURE;
}

#pragma region IWebSocketResource

void WinHTTPWebSocketResource::Connect(const Protocols& protocols, const Options& options)
{
}

void WinHTTPWebSocketResource::Ping() {}

void WinHTTPWebSocketResource::Send(const string& message)
{
}

void WinHTTPWebSocketResource::SendBinary(const string& base64String)
{
}

void WinHTTPWebSocketResource::Close(CloseCode code, const string& reason)
{
}

IWebSocketResource::ReadyState WinHTTPWebSocketResource::GetReadyState() const
{
  return ReadyState::Closed;
}

void WinHTTPWebSocketResource::SetOnConnect(function<void()>&& handler)
{
}

void WinHTTPWebSocketResource::SetOnPing(function<void()>&& handler)
{
}

void WinHTTPWebSocketResource::SetOnSend(function<void(size_t)>&& handler)
{
}

void WinHTTPWebSocketResource::SetOnMessage(function<void(size_t, const string&)>&& handler)
{
}

void WinHTTPWebSocketResource::SetOnClose(function<void(CloseCode, const string&)>&& handler)
{
}

void WinHTTPWebSocketResource::SetOnError(function<void(Error&&)>&& handler)
{
}

#pragma endregion IWebSocketResource
} // namespace Microsoft::React
