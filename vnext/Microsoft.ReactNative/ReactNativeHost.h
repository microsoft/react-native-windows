// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "InstanceCreatedEventArgs.g.h"
#include "InstanceDestroyedEventArgs.g.h"
#include "InstanceLoadedEventArgs.g.h"
#include "ReactNativeHost.g.h"

#include "NativeModulesProvider.h"
#include "ReactHost/React.h"
#include "ReactInstanceSettings.h"
#include "ReactPropertyBag.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct InstanceCreatedEventArgs : InstanceCreatedEventArgsT<InstanceCreatedEventArgs> {
  InstanceCreatedEventArgs() = default;
  InstanceCreatedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context);

  winrt::Microsoft::ReactNative::IReactContext Context() noexcept;

 private:
  winrt::Microsoft::ReactNative::IReactContext m_context;
};

struct InstanceLoadedEventArgs : InstanceLoadedEventArgsT<InstanceLoadedEventArgs> {
  InstanceLoadedEventArgs() = default;
  InstanceLoadedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context, bool failed);

  winrt::Microsoft::ReactNative::IReactContext Context() noexcept;
  bool Failed() noexcept;

 private:
  winrt::Microsoft::ReactNative::IReactContext m_context;
  bool m_failed;
};

struct InstanceDestroyedEventArgs : InstanceDestroyedEventArgsT<InstanceDestroyedEventArgs> {
  InstanceDestroyedEventArgs() = default;
  InstanceDestroyedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context);

  winrt::Microsoft::ReactNative::IReactContext Context() noexcept;

 private:
  winrt::Microsoft::ReactNative::IReactContext m_context;
};

// WinRT ABI-safe implementation of ReactHost.
struct ReactNativeHost : ReactNativeHostT<ReactNativeHost> {
 public: // ReactNativeHost ABI API
  ReactNativeHost() noexcept;

  // property PackageProviders
  Windows::Foundation::Collections::IVector<IReactPackageProvider> PackageProviders() noexcept;

  // property InstanceSettings
  ReactNative::ReactInstanceSettings InstanceSettings() noexcept;
  void InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept;

  Windows::Foundation::IAsyncAction LoadInstance() noexcept;
  Windows::Foundation::IAsyncAction ReloadInstance() noexcept;
  Windows::Foundation::IAsyncAction UnloadInstance() noexcept;

  winrt::event_token InstanceCreated(
      Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceCreatedEventArgs> const
          &handler) noexcept;
  void InstanceCreated(winrt::event_token const &token) noexcept;

  winrt::event_token InstanceLoaded(
      Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceLoadedEventArgs> const
          &handler) noexcept;
  void InstanceLoaded(winrt::event_token const &token) noexcept;

  winrt::event_token InstanceDestroyed(
      Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceDestroyedEventArgs> const
          &handler) noexcept;
  void InstanceDestroyed(winrt::event_token const &token) noexcept;

 public:
  Mso::React::IReactHost *ReactHost() noexcept;
  static winrt::Microsoft::ReactNative::ReactNativeHost GetReactNativeHost(ReactPropertyBag const &properties) noexcept;

 private:
  Mso::CntPtr<Mso::React::IReactHost> m_reactHost;

  winrt::event<Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceCreatedEventArgs>>
      m_instanceCreatedEvent;
  winrt::event<Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceLoadedEventArgs>>
      m_instanceLoadedEvent;
  winrt::event<Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceDestroyedEventArgs>>
      m_instanceDestroyedEvent;

  ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};
  ReactNative::IReactPackageBuilder m_packageBuilder;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {

struct ReactNativeHost : ReactNativeHostT<ReactNativeHost, implementation::ReactNativeHost> {};

} // namespace winrt::Microsoft::ReactNative::factory_implementation
