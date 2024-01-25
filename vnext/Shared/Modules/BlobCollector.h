// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Networking/IBlobResource.h>

// JSI
#include <jsi/jsi.h>

namespace Microsoft::React {

class JSI_EXPORT BlobCollector : public facebook::jsi::HostObject {
  std::string m_blobId;
  std::weak_ptr<Networking::IBlobResource> m_weakResource;

 public:
  BlobCollector(std::string blobId, std::shared_ptr<Networking::IBlobResource> resource) noexcept;

  ~BlobCollector() noexcept;
};

} // namespace Microsoft::React
