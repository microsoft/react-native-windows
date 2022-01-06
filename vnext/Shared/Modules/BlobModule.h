// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Modules/IWebSocketModuleContentHandler.h>

// React Native
#include <cxxreact/CxxModule.h>

// Standard Library
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Microsoft::React {

class BlobModule : public facebook::xplat::module::CxxModule {
  std::shared_ptr<IWebSocketModuleContentHandler> m_contentHandler;

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

#pragma region CxxModule overrides

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

#pragma endregion CxxModule overrides
};

class BlobWebSocketModuleContentHandler final : public IWebSocketModuleContentHandler {
  std::unordered_map<std::string, std::vector<uint8_t>> m_blobs;
  std::mutex m_blobsMutex;
  std::unordered_set<std::int64_t> m_socketIDs;
  std::mutex m_socketIDsMutex;

 public:
#pragma region IWebSocketModuleContentHandler overrides

  void ProcessMessage(std::string &&message, folly::dynamic &params) override;

  void ProcessMessage(std::vector<uint8_t> &&message, folly::dynamic &params) override;

#pragma endregion IWebSocketModuleContentHandler overrides
};

} // namespace Microsoft::React
