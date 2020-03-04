// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"

#include <Unicode.h>
#include "WinHTTPWebSocketResource.h"

// Windows API
#include <comutil.h>

using std::function;
using std::size_t;
using std::string;

namespace Microsoft::React
{

WinHTTPWebSocketResource::WinHTTPWebSocketResource(const string& urlString)
  : m_openFlags{ 0 }
{
  BSTR bstr = _com_util::ConvertStringToBSTR(urlString.c_str());
  LPWSTR lpwstr = bstr;
  SysFreeString(bstr);

  LPURL_COMPONENTS url{};
  auto parseResult = WinHttpCrackUrl(lpwstr, static_cast<DWORD>(urlString.length()), m_openFlags, url);
  if (!parseResult)
    throw std::exception("Could not parse URL.");

  if (wcscmp(L"wss", url->lpszScheme) == 0)
  {
    m_openFlags |= WINHTTP_FLAG_SECURE;
  }
  else if (wcscmp(L"ws", url->lpszScheme) != 0)
  {
    throw std::exception(Common::Unicode::Utf16ToUtf8(
      std::wstring(L"Incorrect URL scheme: ") + std::wstring(url->lpszScheme)
    ).c_str());
  }

  m_url = *url;
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
