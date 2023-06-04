// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Modules/IBlobPersistor.h>

// Standard Library
#include <functional>
#include <string>

namespace Microsoft::React {

struct IFileReaderResource {

  virtual ~IFileReaderResource() noexcept {}

  virtual void ReadAsText(std::string&& blobId, int64_t offset, int64_t size, std::string&& encoding, std::shared_ptr<IBlobPersistor> persistor, std::function<void(std::string&&)>&& resolver, std::function<void(std::string&&)>&& rejecter) noexcept = 0;

  virtual void ReadAsDataUrl(std::string&& blobId, int64_t offset, int64_t size, std::string&& type, std::shared_ptr<IBlobPersistor> persistor, std::function<void(std::string&&)>&& resolver, std::function<void(std::string&&)>&& rejecter) noexcept = 0;

  static std::shared_ptr<IFileReaderResource> Make() noexcept;
};

}// namespace Microsoft::React
