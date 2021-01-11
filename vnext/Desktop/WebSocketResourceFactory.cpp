// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <RuntimeOptions.h>
#include <WinRTWebSocketResource.h>
#include "BeastWebSocketResource.h"

using std::make_shared;
using std::shared_ptr;
using std::string;

namespace Microsoft::React {
#pragma region IWebSocketResource static members

/*static*/
<<<<<<< HEAD
shared_ptr<IWebSocketResource> IWebSocketResource::Make(string &&urlString) {
  if (!GetRuntimeOptionBool("UseBeastWebSocket")) {
    std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult> certExceptions;
    if (GetRuntimeOptionBool("WebSocket.AcceptSelfSigned")) {
      certExceptions.emplace_back(
          winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult::Untrusted);
      certExceptions.emplace_back(
          winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult::InvalidName);
    }
    return make_shared<WinRTWebSocketResource>(std::move(urlString), std::move(certExceptions));
  } else {
    Url url(std::move(urlString));
||||||| 811c767bf
shared_ptr<IWebSocketResource>
IWebSocketResource::Make(const string &urlString, bool legacyImplementation, bool acceptSelfSigned) {
  if (!legacyImplementation) {
    std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult> certExceptions;
    if (acceptSelfSigned) {
      certExceptions.emplace_back(
          winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult::Untrusted);
      certExceptions.emplace_back(
          winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult::InvalidName);
    }
    return make_shared<WinRTWebSocketResource>(urlString, certExceptions);
  } else {
    Url url(urlString);
=======
shared_ptr<IWebSocketResource> IWebSocketResource::Make(string &&urlString) {
#if ENABLE_BEAST
  if (GetRuntimeOptionBool("UseBeastWebSocket")) {
    Url url(std::move(urlString));
>>>>>>> 64b0f8706de05473456eae6340a4cbcd938baaaa

    if (url.scheme == "ws") {
      if (url.port.empty())
        url.port = "80";

      return make_shared<Beast::WebSocketResource>(std::move(url));
    } else if (url.scheme == "wss") {
      if (url.port.empty())
        url.port = "443";

      return make_shared<Beast::SecureWebSocketResource>(std::move(url));
    } else {
      throw std::invalid_argument((string("Incorrect URL scheme: ") + url.scheme).c_str());
    }
  } else {
#endif // ENABLE_BEAST
    std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult> certExceptions;
    if (GetRuntimeOptionBool("WebSocket.AcceptSelfSigned")) {
      certExceptions.emplace_back(
          winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult::Untrusted);
      certExceptions.emplace_back(
          winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult::InvalidName);
    }
    return make_shared<WinRTWebSocketResource>(std::move(urlString), std::move(certExceptions));
#if ENABLE_BEAST
  }
#endif // ENABLE_BEAST
}

#pragma endregion IWebSocketResource static members
} // namespace Microsoft::React
