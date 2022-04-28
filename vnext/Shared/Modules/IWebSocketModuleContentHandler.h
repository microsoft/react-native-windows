// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// React Native
#include <folly/dynamic.h>

// Windows API
#include <winrt/Windows.Foundation.h>

// Standard Library
#include <string>
#include <vector>

#include <unordered_map>
#include <unordered_set>

namespace Microsoft::React {

/// <summary>
/// See https://github.com/facebook/react-native/blob/v0.63.2/React/CoreModules/RCTWebSocketModule.h#L12
/// </summary>
struct IWebSocketModuleContentHandler {
  virtual ~IWebSocketModuleContentHandler() noexcept {}

  virtual void ProcessMessage(std::string &&message, folly::dynamic &params) = 0;

  virtual void ProcessMessage(std::vector<uint8_t> &&message, folly::dynamic &params) = 0;
};

class WebSocketModuleContentHandler
    : public winrt::implements<WebSocketModuleContentHandler, winrt::Windows::Foundation::IInspectable> {
  std::unordered_map<std::string, std::vector<uint8_t>> m_blobs;
  std::mutex m_mutex;
  std::unordered_set<int64_t> m_socketIds;

 public:
  void ProcessMessage(std::string &&message, folly::dynamic &params); // TODO: noexcept?

  void ProcessMessage(std::vector<uint8_t> &&message, folly::dynamic &params); // TODO: noexcept?

  void Register(int64_t socketId) noexcept;

  void Unregister(int64_t socketId) noexcept;

  winrt::array_view<uint8_t> ResolveMessage(std::string &&blogId, int64_t offset, int64_t size) noexcept;

  void StoreMessage(std::vector<uint8_t> &&message, std::string &&blobId) noexcept;

  void RemoveMessage(std::string &&blobId) noexcept;
};
} // namespace Microsoft::React
