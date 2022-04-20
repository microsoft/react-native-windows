// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Networking/WinRTWebSocketResource.h>
#include <CppRuntimeOptions.h>
#if ENABLE_BEAST
#include "BeastWebSocketResource.h"
#endif // ENABLE_BEAST

using std::shared_ptr;
using std::string;

namespace Microsoft::React::Networking {
#pragma region IWebSocketResource static members

/*static*/
shared_ptr<IWebSocketResource> IWebSocketResource::Make() {
#if ENABLE_BEAST
  if (GetRuntimeOptionBool("UseBeastWebSocket")) {
    return std::make_shared<BeastWebSocketResource>();
  } else {
#endif // ENABLE_BEAST
    std::vector<winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult> certExceptions;
    if (GetRuntimeOptionBool("WebSocket.AcceptSelfSigned")) {
      certExceptions.emplace_back(
          winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult::Untrusted);
      certExceptions.emplace_back(
          winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult::InvalidName);
    }
    return std::make_shared<WinRTWebSocketResource>(std::move(certExceptions));
#if ENABLE_BEAST
  }
#endif // ENABLE_BEAST
}

#pragma endregion IWebSocketResource static members
} // namespace Microsoft::React::Networking
