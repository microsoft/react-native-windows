// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Modules/IRequestBodyHandler.h>
#include <Modules/IResponseHandler.h>
#include <Modules/IUriHandler.h>
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

class BlobWebSocketModuleContentHandler final : public IWebSocketModuleContentHandler {
  std::unordered_map<std::string, std::vector<uint8_t>> m_blobs;
  std::mutex m_blobsMutex;
  std::unordered_set<int64_t> m_socketIDs;
  std::mutex m_socketIDsMutex;

 public:
#pragma region IWebSocketModuleContentHandler

  void ProcessMessage(std::string &&message, folly::dynamic &params) override;

  void ProcessMessage(std::vector<uint8_t> &&message, folly::dynamic &params) override;

#pragma endregion IWebSocketModuleContentHandler

  void Register(int64_t socketID) noexcept;

  void Unregister(int64_t socketID) noexcept;

  winrt::array_view<uint8_t> ResolveMessage(std::string &&blobId, int64_t offset, int64_t size) noexcept;

  void RemoveMessage(std::string &&blobId) noexcept;

  void StoreMessage(std::vector<uint8_t> &&message, std::string &&blobId) noexcept;
};

class BlobModuleUriHandler final : public IUriHandler {
#pragma region IUriHandler

  bool Supports(std::string &uri, std::string &responseType) override;

  folly::dynamic Fetch(std::string &uri) override;

#pragma endregion IUriHandler
};

class BlobModuleRequestBodyHandler final : public IRequestBodyHandler {
#pragma region IRequestBodyHandler

  bool Supports(folly::dynamic &data) override;

  void * /*RequestBody*/ ToRequestBody(folly::dynamic &data, std::string &contentType) override;

#pragma endregion IRequestBodyHandler
};

class BlobModuleResponseHandler final : public IResponseHandler {
#pragma region IResponseHandler

  bool Supports(std::string responseType) override;

  folly::dynamic ToResponseData(folly::dynamic &body) override;

#pragma endregion IResponseHandler
};

class BlobModule : public facebook::xplat::module::CxxModule {
  std::shared_ptr<BlobWebSocketModuleContentHandler> m_contentHandler;

  // Property bag high level reference.
  winrt::Windows::Foundation::IInspectable m_iProperties;

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

  BlobModule(winrt::Windows::Foundation::IInspectable const &iProperties) noexcept;

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
