// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <NativeWebSocketModuleSpec.g.h>
#include <Modules/IWebSocketModuleProxy.h>
#include <NativeModules.h>
#include <Networking/IWebSocketResource.h>

namespace Microsoft::React {

class WebSocketTurboModuleProxy final : public IWebSocketModuleProxy {
  std::unordered_map<double, std::shared_ptr<Networking::IWebSocketResource>> &m_resourceMap;

 public:
  WebSocketTurboModuleProxy(
      std::unordered_map<double, std::shared_ptr<Networking::IWebSocketResource>> &resourceMap) noexcept;

#pragma region IWebSocketModuleProxy

  void SendBinary(std::string &&base64String, int64_t id) noexcept override;

#pragma endregion
};

REACT_MODULE(WebSocketTurboModule, L"WebSocketModule")
struct WebSocketTurboModule {
  using ModuleSpec = ReactNativeSpecs::WebSocketModuleSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(Connect, L"connect")
  void Connect(
      std::string &&url,
      std::optional<std::vector<std::string>> protocols,
      ReactNativeSpecs::WebSocketModuleSpec_connect_options &&options,
      double socketID) noexcept;

  REACT_METHOD(Close, L"close")
  void Close(double code, std::string &&reason, double socketID) noexcept;

  REACT_METHOD(Send, L"send")
  void Send(std::string &&message, double forSocketID) noexcept;

  REACT_METHOD(SendBinary, L"sendBinary")
  void SendBinary(std::string &&base64String, double forSocketID) noexcept;

  REACT_METHOD(Ping, L"ping")
  void Ping(double socketID) noexcept;

  REACT_METHOD(AddListener, L"addListener")
  void AddListener(std::string &&eventName) noexcept;

  REACT_METHOD(RemoveListeners, L"removeListeners")
  void RemoveListeners(double count) noexcept;

 private:
  std::shared_ptr<Networking::IWebSocketResource> CreateResource(int64_t id, std::string &&url) noexcept;

  winrt::Microsoft::ReactNative::ReactContext m_context;
  std::unordered_map<double, std::shared_ptr<Networking::IWebSocketResource>> m_resourceMap;

  /// <summary>
  /// Exposes a subset of the module's methods.
  /// </summary>
  std::shared_ptr<IWebSocketModuleProxy> m_proxy;
};

} // namespace Microsoft::React
