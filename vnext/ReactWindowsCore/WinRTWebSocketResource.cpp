// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// clang-format off
#include "WinRTWebSocketResource.h"

using std::function;
using std::size_t;
using std::string;

namespace Microsoft::React
{

#pragma region IWebSocketResource

void IWebSocketResource::Connect(const Protocols& protocols, const Options& options)
{

}

void IWebSocketResource::Ping()
{

}

void IWebSocketResource::Send(const string& message)
{

}

void IWebSocketResource::SendBinary(const string& base64String)
{

}

void IWebSocketResource::Close(CloseCode code, const string& reason)
{

}

IWebSocketResource::ReadyState IWebSocketResource::GetReadyState() const
{
  return IWebSocketResource::ReadyState::Closed;
}

void IWebSocketResource::SetOnConnect(function<void()>&& handler)
{

}

void IWebSocketResource::SetOnPing(function<void()>&& handler)
{

}

void IWebSocketResource::SetOnSend(function<void(size_t)>&& handler)
{

}

void IWebSocketResource::SetOnMessage(function<void(size_t, const string&)>&& handler)
{

}

void IWebSocketResource::SetOnClose(function<void(CloseCode, const string&)>&& handler)
{

}

void IWebSocketResource::SetOnError(function<void(Error&&)>&& handler)
{

}

#pragma endregion IWebSocketResource

}// namespace Microsoft::React
