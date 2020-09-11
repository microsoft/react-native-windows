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
  ~WebSocketModule();

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
  /// Notifies an event to the current React Instance.
  /// </summary>
  void SendEvent(std::string &&eventName, folly::dynamic &&parameters);

  /// <summary>
  /// Creates or retrieves a raw <c>IWebSocketResource</c> pointer.
  /// </summary>
  std::shared_ptr<IWebSocketResource> GetOrCreateWebSocket(std::int64_t id, std::string &&url = {});

  /// <summary>
  /// Keeps <c>IWebSocketResource</c> instances identified by <c>id</c>.
  /// As defined in WebSocket.js.
  /// </summary>
  std::map<int64_t, std::shared_ptr<IWebSocketResource>> m_webSockets;

  /// <summary>
  /// Generates IWebSocketResource instances, defaulting to IWebSocketResource::Make.
  /// </summary>
  std::function<std::shared_ptr<IWebSocketResource>(std::string &&)> m_resourceFactory;
};

} // namespace Microsoft::React
