// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Modules/IBlobPersistor.h>
#include <Modules/IRequestBodyHandler.h>
#include <Modules/IResponseHandler.h>
#include <Modules/IWebSocketModuleContentHandler.h>

// React Native
#include <cxxreact/CxxModule.h>

// Windows API
#include <winrt/base.h>

// Standard Library
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Microsoft::React {

class MemoryBlobPersistor final : public IBlobPersistor {
  std::unordered_map<std::string, std::vector<uint8_t>> m_blobs;
  std::mutex m_mutex;

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

  void ProcessMessage(std::string &&message, folly::dynamic &params) override;

  void ProcessMessage(std::vector<uint8_t> &&message, folly::dynamic &params) override;

#pragma endregion IWebSocketModuleContentHandler

  void Register(int64_t socketID) noexcept;

  void Unregister(int64_t socketID) noexcept;
};

class BlobModuleRequestBodyHandler final : public IRequestBodyHandler {
  std::shared_ptr<IBlobPersistor> m_blobPersistor;

 public:
  BlobModuleRequestBodyHandler(std::shared_ptr<IBlobPersistor> blobPersistor) noexcept;

#pragma region IRequestBodyHandler

  bool Supports(folly::dynamic &data) override;

  folly::dynamic ToRequestBody(folly::dynamic &data, std::string &contentType) override;

#pragma endregion IRequestBodyHandler
};

class BlobModuleResponseHandler final : public IResponseHandler {
  std::shared_ptr<IBlobPersistor> m_blobPersistor;

 public:
  BlobModuleResponseHandler(std::shared_ptr<IBlobPersistor> blobPersistor) noexcept;

#pragma region IResponseHandler

  bool Supports(std::string &responseType) override;

  folly::dynamic ToResponseData(std::vector<uint8_t> &&content) override;

#pragma endregion IResponseHandler
};

class BlobModule : public facebook::xplat::module::CxxModule {
  std::shared_ptr<MemoryBlobPersistor> m_blobPersistor;
  std::shared_ptr<BlobWebSocketModuleContentHandler> m_contentHandler;
  std::shared_ptr<BlobModuleRequestBodyHandler> m_requestBodyHandler;
  std::shared_ptr<BlobModuleResponseHandler> m_responseHandler;

  // Property bag high level reference.
  winrt::Windows::Foundation::IInspectable m_inspectableProperties;

 public:
  enum class MethodId {
    AddNetworkingHandler = 0,
    AddWebSocketHandler = 1,
    RemoveWebSocketHandler = 2,
    SendOverSocket = 3,
    CreateFromParts = 4,
    Release = 5,
    SIZE = 6
  };

  BlobModule(winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept;

  ~BlobModule() noexcept override;

  struct SharedState {
    /// <summary>
    /// Keeps a raw reference to the module object to lazily retrieve the React Instance as needed.
    /// </summary>
    CxxModule *Module{nullptr};
  };

#pragma region CxxModule

  /// <summary>
  /// <see cref="facebook::xplat::module::CxxModule::getName" />
  /// </summary>
  std::string getName() override;

  /// <summary>
  /// <see cref="facebook::xplat::module::CxxModule::getConstants" />
  /// </summary>
  std::map<std::string, folly::dynamic> getConstants() override;

  /// <summary>
  /// <see cref="facebook::xplat::module::CxxModule::getMethods" />
  /// </summary>
  /// <remarks>See See react-native/Libraries/WebSocket/WebSocket.js</remarks>
  std::vector<Method> getMethods() override;

#pragma endregion CxxModule

 private:
  /// <summary>
  /// Keeps members that can be accessed threads other than this module's owner accessible.
  /// </summary>
  std::shared_ptr<SharedState> m_sharedState;
};

} // namespace Microsoft::React
