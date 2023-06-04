// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IFileReaderResource.h"

// Standard Library
#include <memory>

namespace Microsoft::React {

class BaseFileReaderResource : public IFileReaderResource, public std::enable_shared_from_this<BaseFileReaderResource> {

public:
#pragma region IFileReaderResource

  void ReadAsText(std::string&& blobId, int64_t offset, int64_t size, std::string&& encoding, std::shared_ptr<IBlobPersistor> persistor, std::function<void(std::string&&)>&& resolver, std::function<void(std::string&&)>&& rejecter) noexcept override;

  void ReadAsDataUrl(std::string&& blobId, int64_t offset, int64_t size, std::string&& type, std::shared_ptr<IBlobPersistor> persistor, std::function<void(std::string&&)>&& resolver, std::function<void(std::string&&)>&& rejecter) noexcept override;

#pragma endregion IFileReaderResource

};

}// namespace Microsoft::React
