// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

// #include <NativeFileReaderModuleSpec.g.h>

#include <Modules/IWebSocketModuleProxy.h>
#include <NativeModules.h>
#include <Networking/IWebSocketResource.h>

namespace Microsoft::React {

REACT_MODULE(WebSocketTurboModule, L"WebSocketModule")
struct WebSocketTurboModule {
  // using ModuleSpec = ReactNativeSpecs::WebSocketModuleSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(Connect, L"connect")
  void Connect(
      std::string &&url,
      winrt::Microsoft::ReactNative::JSValueArray &&protocols,
      winrt::Microsoft::ReactNative::JSValueObject &&options,
      int64_t id,
      winrt::Microsoft::ReactNative::ReactPromise<std::string> &&result) noexcept;

 private:
  std::shared_ptr<Networking::IWebSocketResource> CreateResource(int64_t id, std::string &&url) noexcept;

  winrt::Microsoft::ReactNative::ReactContext m_context;
  std::unordered_map<int64_t, std::shared_ptr<Networking::IWebSocketResource>> m_resourceMap;
};

} // namespace Microsoft::React
