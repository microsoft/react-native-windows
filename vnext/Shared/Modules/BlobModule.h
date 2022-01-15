// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

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

class BlobModule : public facebook::xplat::module::CxxModule {
  std::shared_ptr<BlobWebSocketModuleContentHandler> m_contentHandler;

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

  BlobModule() noexcept;

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
