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
  static std::weak_ptr<IWebSocketModuleContentHandler> GetInstance() noexcept;

  virtual ~IWebSocketModuleContentHandler() noexcept {}

  virtual void Register(std::int64_t socketID) noexcept = 0;

  virtual void Unregister(std::int64_t socketID) noexcept = 0;

  virtual bool IsRegistered(std::int64_t socketID) noexcept = 0;

  virtual std::vector<std::uint8_t>
  ResolveMessage(std::string &&blobId, std::int64_t offset, std::int64_t size) noexcept = 0;

  virtual void RemoveMessage(std::string &&blobId) noexcept = 0;

  virtual void ProcessMessage(std::string &&message, folly::dynamic &params) = 0;

  virtual void ProcessMessage(std::vector<std::uint8_t> &&message, folly::dynamic &params) = 0;

  virtual void StoreMessage(std::vector<std::uint8_t> &&message, std::string &&blobId) noexcept = 0;
};

} // namespace Microsoft::React
