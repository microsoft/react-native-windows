// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"

#include <Unicode.h>
#include "BeastWebSocketResource.h"
#include "WinHTTPWebSocketResource.h"

// Windows API
#include <comutil.h>

using std::string;
using std::unique_ptr;

namespace Microsoft::React
{
#pragma region IWebSocketResource static members

/*static*/ unique_ptr<IWebSocketResource> IWebSocketResource::Make(const string &urlString) {
  if (true)
  {
    BSTR bstr = _com_util::ConvertStringToBSTR(urlString.c_str());
    LPWSTR lpwstr = bstr;
    SysFreeString(bstr);

    LPURL_COMPONENTS url{};
    DWORD urlFlags{ 0 };
    auto parseResult = WinHttpCrackUrl(lpwstr, static_cast<DWORD>(urlString.length()), urlFlags, url);

    if (!parseResult)
      return nullptr;

    if (wcscmp(url->lpszScheme, L"ws") == 0)
      return unique_ptr<IWebSocketResource>(new WinHTTPWebSocketResource(*url, false));
    else if (wcscmp(url->lpszScheme, L"wss") == 0)
      return unique_ptr<IWebSocketResource>(new WinHTTPWebSocketResource(*url, true));

    throw std::exception(Common::Unicode::Utf16ToUtf8(
      std::wstring(L"Incorrect URL scheme: ") + std::wstring(url->lpszScheme)
    ).c_str());
  }
  else
  {
    Url url(urlString);

    if (url.scheme == "ws") {
      if (url.port.empty())
        url.port = "80";

      return unique_ptr<IWebSocketResource>(new Beast::WebSocketResource(std::move(url)));
    }
    else if (url.scheme == "wss") {
      if (url.port.empty())
        url.port = "443";

      return unique_ptr<IWebSocketResource>(new Beast::SecureWebSocket(std::move(url)));
    }
    else
      throw std::exception((string("Incorrect URL scheme: ") + url.scheme).c_str());
  }
}

#pragma endregion IWebSocketResource static members
}
