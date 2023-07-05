// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "IBlobResource.h"

#include <Modules/IBlobPersistor.h>
#include <Modules/IWebSocketModuleContentHandler.h>

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

class BlobWebSocketModuleContentHandler final : public IWebSocketModuleContentHandler {
  std::unordered_set<int64_t> m_socketIds;
  std::mutex m_mutex;
  std::shared_ptr<IBlobPersistor> m_blobPersistor;

public:
  BlobWebSocketModuleContentHandler(std::shared_ptr<IBlobPersistor> blobPersistor) noexcept;

#pragma region IWebSocketModuleContentHandler

  void ProcessMessage(std::string&& message, folly::dynamic& params) override;

  void ProcessMessage(std::vector<uint8_t>&& message, folly::dynamic& params) override;

  void ProcessMessage(std::string&& message, winrt::Microsoft::ReactNative::JSValueObject& params) noexcept override;

  void ProcessMessage(std::vector<uint8_t>&& message, winrt::Microsoft::ReactNative::JSValueObject& params) noexcept
    override;

#pragma endregion IWebSocketModuleContentHandler

  void Register(int64_t socketID) noexcept;

  void Unregister(int64_t socketID) noexcept;
};

class DefaultBlobResource : public IBlobResource, public std::enable_shared_from_this<DefaultBlobResource> {
  std::shared_ptr<MemoryBlobPersistor> m_blobPersistor;
  std::shared_ptr<BlobWebSocketModuleContentHandler> m_contentHandler;
  BlobCallbacks m_callbacks;

 public:
#pragma region IBlobResource

  void SendOverSocket(std::string &&blobId, int64_t offset, int64_t size, int64_t socketId) noexcept override;

  void CreateFromParts(winrt::Microsoft::ReactNative::JSValueArray &&parts, std::string &&blobId) noexcept override;

  void Release(std::string &&blobId) noexcept override;

  BlobCallbacks &Callbacks() noexcept override;

#pragma endregion IBlobResource
};

} // namespace Microsoft::React::Networking
