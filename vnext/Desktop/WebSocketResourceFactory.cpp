// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <CppRuntimeOptions.h>
#include <Networking/WinRTWebSocketResource.h>

using std::shared_ptr;
using std::string;
using winrt::Windows::Security::Cryptography::Certificates::ChainValidationResult;

namespace Microsoft::React::Networking {
#pragma region IWebSocketResource static members

/*static*/
shared_ptr<IWebSocketResource> IWebSocketResource::Make() {
  std::vector<ChainValidationResult> certExceptions;
  if (GetRuntimeOptionBool("WebSocket.AcceptSelfSigned")) {
    certExceptions.emplace_back(ChainValidationResult::Untrusted);
    certExceptions.emplace_back(ChainValidationResult::InvalidName);
  }

  // if (GetRuntimeOptionBool("WebSocket.ResourceV2")) {
  return std::make_shared<WinRTWebSocketResource2>(std::move(certExceptions));
  // } else {
  //   return std::make_shared<WinRTWebSocketResource>(std::move(certExceptions));
  // }
}

#pragma endregion IWebSocketResource static members
} // namespace Microsoft::React::Networking
