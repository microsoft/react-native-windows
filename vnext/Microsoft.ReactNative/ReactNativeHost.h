// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ReactNativeHost.g.h"

#include "NativeModulesProvider.h"
#include "ReactHost/React.h"
#include "ReactInstanceSettings.h"
#include "ReactPropertyBag.h"

namespace winrt::Microsoft::ReactNative::implementation {

// WinRT ABI-safe implementation of ReactHost.
struct ReactNativeHost : ReactNativeHostT<ReactNativeHost> {
 public: // ReactNativeHost ABI API
  ReactNativeHost() noexcept;

  static ReactNative::ReactNativeHost FromContext(ReactNative::IReactContext const &reactContext) noexcept;

  // property PackageProviders
  Windows::Foundation::Collections::IVector<IReactPackageProvider> PackageProviders() noexcept;

  // property InstanceSettings
  ReactNative::ReactInstanceSettings InstanceSettings() noexcept;
  void InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept;

  Windows::Foundation::IAsyncAction LoadInstance() noexcept;
  Windows::Foundation::IAsyncAction ReloadInstance() noexcept;
  Windows::Foundation::IAsyncAction UnloadInstance() noexcept;

 public:
  Mso::React::IReactHost *ReactHost() noexcept;
  static ReactNative::ReactNativeHost GetReactNativeHost(ReactPropertyBag const &properties) noexcept;

 private:
  Mso::CntPtr<Mso::React::IReactHost> m_reactHost;

  ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};
  ReactNative::IReactPackageBuilder m_packageBuilder;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactNativeHost : ReactNativeHostT<ReactNativeHost, implementation::ReactNativeHost> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
