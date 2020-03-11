// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"

#include <WinRTWebSocketResource.h>
#include "BeastWebSocketResource.h"
#include "WinHTTPWebSocketResource.h"

using std::string;
using std::unique_ptr;

namespace Microsoft::React
{
#pragma region IWebSocketResource static members

/*static*/ unique_ptr<IWebSocketResource> IWebSocketResource::Make(const string &urlString) {
  if (true) //TODO: Feature-gate this.
  {
    //return unique_ptr<IWebSocketResource>(new WinHTTPWebSocketResource(urlString));
    return unique_ptr<IWebSocketResource>(new WinRTWebSocketResource(urlString));
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
