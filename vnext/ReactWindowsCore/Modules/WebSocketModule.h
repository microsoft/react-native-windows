// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/CxxModule.h>
#include "IWebSocket.h"

namespace Microsoft::React {

///
/// Realizes <c>NativeModules</c> projection.
/// <remarks>See react-native/Libraries/WebSocket/WebSocket.js</remarks>
///
class WebSocketModule : public facebook::xplat::module::CxxModule {
 public:
  enum MethodId { Connect = 0, Close = 1, Send = 2, SendBinary = 3, Ping = 4, SIZE = 5 };

  WebSocketModule();

  /// <summary>
  /// <see cref="facebook::xplat::module::CxxModule::getName" />
  /// </summary>
  std::string getName();

  /// <summary>
  /// <see cref="facebook::xplat::module::CxxModule::getConstants" />
  /// </summary>
  virtual std::map<std::string, folly::dynamic> getConstants();

  /// <summary>
  /// <see cref="facebook::xplat::module::CxxModule::getMethods" />
  /// </summary>
  /// <remarks>See See react-native/Libraries/WebSocket/WebSocket.js</remarks>
  virtual std::vector<Method> getMethods();

 private:
  /// <summary>
  /// Notifies an event to the current React Instance.
  /// </summary>
  void SendEvent(std::string &&eventName, folly::dynamic &&parameters);

  /// <summary>
  /// Creates or retrieves a raw <c>IWebSocket</c> pointer.
  /// </summary>
  IWebSocket *GetOrCreateWebSocket(int64_t id, std::string &&url = std::string());

  /// <summary>
  /// Keeps <c>IWebSocket</c> instances identified by <c>id</c>.
  /// As defined in WebSocket.js.
  /// </summary>
  std::map<int64_t, std::unique_ptr<IWebSocket>> m_webSockets;
};

} // namespace Microsoft::React

// Deprecated. Keeping for compatibility.
namespace facebook::react {

using WebSocketModule = Microsoft::React::WebSocketModule;

} // namespace facebook::react
