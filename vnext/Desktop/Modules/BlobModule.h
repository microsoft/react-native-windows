// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IWebSocketContentHandler.h>

// React Native
#include <cxxreact/CxxModule.h>

// Standard Library
#include <unordered_map>
#include <mutex>
#include <string>
#include <vector>

namespace Microsoft::React {

class BlobModule : public facebook::xplat::module::CxxModule, IWebSocketContentHandler {
  std::unordered_map<std::string, std::vector<std::uint8_t>> m_blobs;
  std::mutex m_blobsMutex;

 public:
  enum MethodId {
    AddNetworkingHandler = 0,
    AddWebSocketHandler = 1,
    RemoveWebSocketHandler = 2,
    SendOverSocket = 3,
    CreateFromParts = 4,
    Release = 5,
    SIZE = 6
  };

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

#pragma region IWebSocketContentHandler overrides

  void ProcessMessage(std::string&& message, folly::dynamic &params) override;

  void ProcessMessage(std::vector<std::uint8_t>&& message, folly::dynamic &params) override;

#pragma endregion IWebSocketContentHandler overrides
};

} // namespace Microsoft::React
