// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// React Native
#include <folly/dynamic.h>

// Standard Library
#include <string>
#include <vector>

namespace Microsoft::React {

/// <summary>
/// See https://github.com/facebook/react-native/blob/v0.63.2/React/CoreModules/RCTWebSocketModule.h#L12
/// </summary>
struct IWebSocketModuleContentHandler {
  virtual ~IWebSocketModuleContentHandler() noexcept {}

  virtual void ProcessMessage(std::string &&message, folly::dynamic &params) = 0;

  virtual void ProcessMessage(std::vector<uint8_t> &&message, folly::dynamic &params) = 0;
};

} // namespace Microsoft::React
