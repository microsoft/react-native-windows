// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include "IWebSocketResource.h"

namespace Microsoft::React {

///
/// Realizes <c>NativeModules</c> projection.
/// <remarks>See react-native/Libraries/WebSocket/WebSocket.js</remarks>
///
class WebSocketModule : public facebook::xplat::module::CxxModule {
 public:
  enum MethodId { Connect = 0, Close = 1, Send = 2, SendBinary = 3, Ping = 4, SIZE = 5 };

  WebSocketModule();

  ~WebSocketModule() noexcept override;

  struct SharedState {
    /// <summary>
    /// Keeps <c>IWebSocketResource</c> instances identified by <c>id</c>.
    /// As defined in WebSocket.js.
    /// </summary>
    std::map<int64_t, std::shared_ptr<IWebSocketResource>> ResourceMap{};

    /// <summary>
    /// Keeps a raw reference to the module object to lazily retrieve the React Instance as needed.
    /// </summary>
    CxxModule *Module{nullptr};
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

  void SetResourceFactory(std::function<std::shared_ptr<IWebSocketResource>(const std::string &)> &&resourceFactory);

 private:
  /// <summary>
  /// Keeps <c>IWebSocketResource</c> instances identified by <c>id</c>.
  /// As defined in WebSocket.js.
  /// </summary>
  std::map<int64_t, std::shared_ptr<IWebSocketResource>> m_webSockets;

  /// <summary>
  /// Generates IWebSocketResource instances, defaulting to IWebSocketResource::Make.
  /// </summary>
  std::function<std::shared_ptr<IWebSocketResource>(std::string &&)> m_resourceFactory;

  /// <summary>
  /// Keeps members that can be accessed threads other than this module's owner accessible.
  /// </summary>
  std::shared_ptr<SharedState> m_sharedState;
};

} // namespace Microsoft::React
