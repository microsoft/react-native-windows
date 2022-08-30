// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// Windows API
#include <winrt/base.h>

// Standard Library
#include <string>
#include <vector>

namespace Microsoft::React {

struct IBlobPersistor {
  ///
  /// <exception cref="std::invalid_argument">
  /// When an entry for blobId cannot be found.
  /// </exception>
  ///
  virtual winrt::array_view<uint8_t const> ResolveMessage(std::string &&blobId, int64_t offset, int64_t size) = 0;

  virtual void RemoveMessage(std::string &&blobId) noexcept = 0;

  virtual void StoreMessage(std::vector<uint8_t> &&message, std::string &&blobId) noexcept = 0;

  virtual std::string StoreMessage(std::vector<uint8_t> &&message) noexcept = 0;
};

} // namespace Microsoft::React
