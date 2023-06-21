// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

//#include <NativeFileReaderModuleSpec.g.h>

#include <Modules/IWebSocketModuleProxy.h>
#include <Networking/IWebSocketResource.h>
#include <NativeModules.h>

namespace Microsoft::React {

REACT_MODULE(WebSocketTurboModule, L"WebSocketModule")
struct WebSocketTurboModule {
  //using ModuleSpec = ReactNativeSpecs::WebSocketModuleSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const& reactContext) noexcept;

  //REACT_METHOD(Connect, L"connect")
  //void Connect(std::string &&url, winrt::Microsoft::ReactNative::JSValueArray &&protocols)
};

} // namespace Microsoft::React
