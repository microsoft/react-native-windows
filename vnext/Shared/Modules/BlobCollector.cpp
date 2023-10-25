// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BlobCollector.h"

using std::string;

namespace Microsoft::React {

using Networking::IBlobResource;

BlobCollector::BlobCollector(string blobId, std::shared_ptr<IBlobResource> resource) noexcept
    : m_blobId{blobId}, m_weakResource{std::weak_ptr<IBlobResource>(resource)} {}

BlobCollector::~BlobCollector() noexcept {
  if (auto rc = m_weakResource.lock()) {
    rc->Release(std::move(m_blobId));
  }
}

} // namespace Microsoft::React
