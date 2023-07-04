// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Folly
#include <folly/dynamic.h>

// React Native Windows
#include <JSValue.h>

// Standard Library
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace Microsoft::React::Networking {

struct IBlobResource {

  virtual void SendOverSocket(
    std::string &&blobId,
    int64_t offset,
    int64_t size,
    int64_t socketId) noexcept = 0;

  virtual void CreateFromParts(folly::dynamic &&parts, std::string&& blobId) noexcept = 0;

  virtual void CreateFromParts(
    winrt::Microsoft::ReactNative::JSValueObject &&parts,
    std::string &&blobId) noexcept = 0;

  virtual void Release(std::string &&blobId) noexcept = 0;

  //TODO: addNetworkingHandler? addWebSocketHandler? removeWebSocketHandler?
};

static std::shared_ptr<IBlobResource> Make();

} // namespace Microsoft::React::Networking
