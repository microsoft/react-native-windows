// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IBlobResource.h"

#include <Modules/IBlobPersistor.h>

// Boost Libraries
#include <boost/uuid/uuid_generators.hpp>

namespace Microsoft::React::Networking {

class MemoryBlobPersistor final : public IBlobPersistor {
  std::unordered_map<std::string, std::vector<uint8_t>> m_blobs;
  std::mutex m_mutex;
  boost::uuids::random_generator m_guidGenerator;

 public:
#pragma region IBlobPersistor

  winrt::array_view<uint8_t const> ResolveMessage(std::string &&blobId, int64_t offset, int64_t size) override;

  void RemoveMessage(std::string &&blobId) noexcept override;

  void StoreMessage(std::vector<uint8_t> &&message, std::string &&blobId) noexcept override;

  std::string StoreMessage(std::vector<uint8_t> &&message) noexcept override;

#pragma endregion IBlobPersistor
};


class DefaultBlobResource : public std::enable_shared_from_this<DefaultBlobResource> {
  std::shared_ptr<MemoryBlobPersistor> m_blobPersistor;

public:
#pragma region IBlobResource

  virtual void SendOverSocket(
    std::string&& blobId,
    int64_t offset,
    int64_t size,
    int64_t socketId) noexcept = 0;

  virtual void CreateFromParts(
    winrt::Microsoft::ReactNative::JSValueArray&& parts,
    std::string&& blobId) noexcept = 0;

  virtual void Release(std::string&& blobId) noexcept = 0;

#pragma endregion IBlobResource

};

} // Microsoft::React::Networking
