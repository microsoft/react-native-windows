// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "NetworkPropertyIds.h"

namespace Microsoft::React {

const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IWebSocketModuleContentHandler>>>
BlobModuleContentHandlerPropertyId() noexcept {
  static const winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IWebSocketModuleContentHandler>>>
      prop{L"BlobModule.ContentHandler"};
  return prop;
}

const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IBlobPersistor>>>
BlobModulePersistorPropertyId() noexcept {
  static const winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IBlobPersistor>>>
      prop{L"Blob.Persistor"};
  return prop;
}

const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IWebSocketModuleProxy>>>
WebSocketModuleProxyPropertyId() noexcept {
  static const winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IWebSocketModuleProxy>>>
      prop{L"WebSocketModule.Proxy"};
  return prop;
}

const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IHttpModuleProxy>>>
HttpModuleProxyPropertyId() noexcept {
  static const winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<IHttpModuleProxy>>>
      prop{L"HttpModule.Proxy"};
  return prop;
}

const winrt::Microsoft::ReactNative::ReactPropertyId<
    winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<Networking::IBlobResource>>>
BlobResourcePropertyId() noexcept {
  static const winrt::Microsoft::ReactNative::ReactPropertyId<
      winrt::Microsoft::ReactNative::ReactNonAbiValue<std::weak_ptr<Networking::IBlobResource>>>
      prop{L"Blob.Resource"};
  return prop;
}

const winrt::Microsoft::ReactNative::ReactPropertyId<winrt::hstring> DefaultUserAgentPropertyId() noexcept {
  static const winrt::Microsoft::ReactNative::ReactPropertyId<winrt::hstring> prop{L"ReactNative", L"UserAgent"};
  return prop;
}

} // namespace Microsoft::React
