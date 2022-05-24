// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Standard Library
#include <memory>
#include <string>

namespace Microsoft::React {

/// <summary>
/// Provides partial access to WebSocketModule methods directly to other native modules
/// without switching to the JavaScript queue thread.
/// </summary>
struct IWebSocketModuleProxy {
  virtual ~IWebSocketModuleProxy() noexcept {}

  virtual void SendBinary(std::string &&base64String, int64_t id) noexcept = 0;
};

} // namespace Microsoft::React
