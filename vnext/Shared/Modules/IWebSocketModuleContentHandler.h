// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// React Native Windows
#include <JSValue.h>

// Standard Library
#include <string>
#include <vector>

namespace Microsoft::React {

/// <summary>
/// See https://github.com/facebook/react-native/blob/v0.63.2/React/CoreModules/RCTWebSocketModule.h#L12
/// </summary>
struct IWebSocketModuleContentHandler {
  virtual ~IWebSocketModuleContentHandler() noexcept {}

  /// Returns true if this handler should process messages for the given socket.
  /// Default returns true for backward compatibility; BlobModule overrides to
  /// check whether binaryType='blob' was set for this socket via addWebSocketHandler.
  ///
  /// WARNING: Subclasses that override Supports() with a stateful or lock-protected
  /// check MUST also override both TryProcessMessage() overloads to perform the
  /// check-and-process atomically. The default TryProcessMessage() calls Supports()
  /// and ProcessMessage() as two separate operations with no lock held between them.
  virtual bool Supports(int64_t /*socketId*/) noexcept {
    return true;
  }

  virtual void ProcessMessage(std::string &&message, winrt::Microsoft::ReactNative::JSValueObject &params) noexcept = 0;

  virtual void ProcessMessage(
      std::vector<uint8_t> &&message,
      winrt::Microsoft::ReactNative::JSValueObject &params) noexcept = 0;

  /// Check Supports() then ProcessMessage() in one call.
  /// Returns true if the message was handled.
  ///
  /// The default implementation does NOT hold any lock across both operations.
  /// Subclasses with a stateful Supports() MUST override these to make the
  /// check-and-process atomic (see BlobWebSocketModuleContentHandler for an example).
  virtual bool TryProcessMessage(
      int64_t socketId,
      std::string &&message,
      winrt::Microsoft::ReactNative::JSValueObject &params) noexcept {
    if (!Supports(socketId))
      return false;
    ProcessMessage(std::move(message), params);
    return true;
  }

  virtual bool TryProcessMessage(
      int64_t socketId,
      std::vector<uint8_t> &&message,
      winrt::Microsoft::ReactNative::JSValueObject &params) noexcept {
    if (!Supports(socketId))
      return false;
    ProcessMessage(std::move(message), params);
    return true;
  }
};

} // namespace Microsoft::React
