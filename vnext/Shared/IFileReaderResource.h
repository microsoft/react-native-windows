// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IBlobPersistor.h"

// Standard Library
#include <functional>
#include <string>

namespace Microsoft::React {

struct IFileReaderResource {
  virtual ~IFileReaderResource() noexcept {}

  virtual void ReadAsText(
      std::string &&blobId,
      int64_t offset,
      int64_t size,
      std::string &&encoding,
      std::function<void(std::string &&)> &&resolver,
      std::function<void(std::string &&)> &&rejecter) noexcept = 0;

  virtual void ReadAsDataUrl(
      std::string &&blobId,
      int64_t offset,
      int64_t size,
      std::string &&type,
      std::function<void(std::string &&)> &&resolver,
      std::function<void(std::string &&)> &&rejecter) noexcept = 0;

  static std::shared_ptr<IFileReaderResource> Make(std::weak_ptr<IBlobPersistor> weakBlobPersistor) noexcept;
};

} // namespace Microsoft::React
