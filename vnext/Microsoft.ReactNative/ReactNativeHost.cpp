// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactNativeHost.h"
#include "ReactNativeHost.g.cpp"

#include "ReactPackageBuilder.h"
#include "RedBox.h"
#include "TurboModulesProvider.h"

#include <future/futureWinRT.h>
#include <winrt/Windows.Foundation.Collections.h>
#include "IReactContext.h"
#include "ReactInstanceSettings.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

#ifndef CORE_ABI
using namespace xaml;
using namespace xaml::Controls;
#endif

namespace winrt::Microsoft::ReactNative::implementation {

InstanceCreatedEventArgs::InstanceCreatedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context)
    : m_context(nullptr /*winrt::make<winrt::Microsoft::ReactNative::IReactContext>(std::move(context))*/) {}

winrt::Microsoft::ReactNative::IReactContext InstanceCreatedEventArgs::Context() noexcept {
  return m_context;
}

InstanceLoadedEventArgs::InstanceLoadedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context, bool failed)
    : m_context(nullptr /*winrt::make<IReactContext>(std::move(context))*/), m_failed(failed) {}

bool InstanceLoadedEventArgs::Failed() noexcept {
  return m_failed;
}

winrt::Microsoft::ReactNative::IReactContext InstanceLoadedEventArgs::Context() noexcept {
  return m_context;
}

InstanceDestroyedEventArgs::InstanceDestroyedEventArgs(Mso::CntPtr<Mso::React::IReactContext> &&context)
    : m_context(nullptr /*winrt::make<IReactContext>(std::move(context))*/) {}

winrt::Microsoft::ReactNative::IReactContext InstanceDestroyedEventArgs::Context() noexcept {
  return m_context;
}

ReactNativeHost::ReactNativeHost() noexcept : m_reactHost{Mso::React::MakeReactHost()} {
#if _DEBUG
  facebook::react::InitializeLogging([](facebook::react::RCTLogLevel /*logLevel*/, const char *message) {
    std::string str = std::string("ReactNative:") + message;
    OutputDebugStringA(str.c_str());
  });
#endif
}

IVector<IReactPackageProvider> ReactNativeHost::PackageProviders() noexcept {
  return InstanceSettings().PackageProviders();
}

ReactNative::ReactInstanceSettings ReactNativeHost::InstanceSettings() noexcept {
  if (!m_instanceSettings) {
    m_instanceSettings = make<ReactInstanceSettings>();
  }

  return m_instanceSettings;
}

void ReactNativeHost::InstanceSettings(ReactNative::ReactInstanceSettings const &value) noexcept {
  m_instanceSettings = value;
}

IAsyncAction ReactNativeHost::LoadInstance() noexcept {
  return ReloadInstance();
}

::Microsoft::ReactNative::ReactPropertyId<::Microsoft::ReactNative::ReactNonAbiValue<winrt::weak_ref<ReactNativeHost>>>
ReactNativeHostProperty() noexcept {
  static ::Microsoft::ReactNative::ReactPropertyId<
      ::Microsoft::ReactNative::ReactNonAbiValue<winrt::weak_ref<ReactNativeHost>>>
      propId{L"ReactNative.ReactNativeHost", L"ReactNativeHost"};
  return propId;
}

/*static*/ winrt::Microsoft::ReactNative::ReactNativeHost ReactNativeHost::GetReactNativeHost(
    ReactPropertyBag const &properties) noexcept {
  if (auto wkHost = properties.Get(ReactNativeHostProperty()).Value()) {
    if (auto abiHost = wkHost.get()) {
      return abiHost.as<winrt::Microsoft::ReactNative::ReactNativeHost>();
    }
  }
  return nullptr;
}

IAsyncAction ReactNativeHost::ReloadInstance() noexcept {
  auto modulesProvider = std::make_shared<NativeModulesProvider>();

#ifndef CORE_ABI
  auto viewManagersProvider = std::make_shared<ViewManagersProvider>();
#endif

  auto turboModulesProvider = std::make_shared<TurboModulesProvider>();

  if (!m_packageBuilder) {
    m_packageBuilder = make<ReactPackageBuilder>(
        modulesProvider,
#ifndef CORE_ABI
        viewManagersProvider,
#endif
        turboModulesProvider);

    if (auto packageProviders = InstanceSettings().PackageProviders()) {
      for (auto const &packageProvider : packageProviders) {
        packageProvider.CreatePackage(m_packageBuilder);
      }
    }
  }

  ReactPropertyBag(m_instanceSettings.Properties()).Set(ReactNativeHostProperty(), get_weak());

  Mso::React::ReactOptions reactOptions{};
  reactOptions.Properties = m_instanceSettings.Properties();
  reactOptions.Notifications = m_instanceSettings.Notifications();
  reactOptions.SetUseDeveloperSupport(m_instanceSettings.UseDeveloperSupport());
  reactOptions.DeveloperSettings.SourceBundleName = to_string(m_instanceSettings.DebugBundlePath());
  reactOptions.SetUseWebDebugger(m_instanceSettings.UseWebDebugger());
  reactOptions.SetUseDirectDebugger(m_instanceSettings.UseDirectDebugger());
  reactOptions.SetDebuggerBreakOnNextLine(m_instanceSettings.DebuggerBreakOnNextLine());
  reactOptions.SetUseFastRefresh(m_instanceSettings.UseFastRefresh());
  reactOptions.SetUseLiveReload(m_instanceSettings.UseLiveReload());
  reactOptions.EnableJITCompilation = m_instanceSettings.EnableJITCompilation();
  reactOptions.DeveloperSettings.DebugHost = to_string(m_instanceSettings.DebugHost());
  reactOptions.BundleRootPath = to_string(m_instanceSettings.BundleRootPath());
  reactOptions.DeveloperSettings.DebuggerPort = m_instanceSettings.DebuggerPort();
  if (m_instanceSettings.RedBoxHandler()) {
    reactOptions.RedBoxHandler = Mso::React::CreateRedBoxHandler(m_instanceSettings.RedBoxHandler());
  }
  reactOptions.DeveloperSettings.SourceBundleHost = to_string(m_instanceSettings.SourceBundleHost());
  reactOptions.DeveloperSettings.SourceBundlePort = m_instanceSettings.SourceBundlePort();

  reactOptions.ByteCodeFileUri = to_string(m_instanceSettings.ByteCodeFileUri());
  reactOptions.EnableByteCodeCaching = m_instanceSettings.EnableByteCodeCaching();
  reactOptions.UseJsi = m_instanceSettings.UseJsi();

  reactOptions.ModuleProvider = modulesProvider;
#ifndef CORE_ABI
  reactOptions.ViewManagerProvider = viewManagersProvider;
#endif
  reactOptions.TurboModuleProvider = turboModulesProvider;

  reactOptions.OnInstanceCreated = [weak = get_weak()](
                                       Mso::CntPtr<Mso::React::IReactContext> &&context, Mso::React::IReactInstance &) {
    if (auto strong = weak.get()) {
      strong->m_instanceCreatedEvent(*strong, winrt::make<InstanceCreatedEventArgs>(std::move(context)));
    }
  };
  reactOptions.OnInstanceLoaded = [weak = get_weak()](
                                      Mso::CntPtr<Mso::React::IReactContext> &&context,
                                      Mso::React::IReactInstance &instance,
                                      const Mso::ErrorCode &err) {
    if (auto strong = weak.get()) {
      strong->m_instanceLoadedEvent(*strong, winrt::make<InstanceLoadedEventArgs>(std::move(context), !!err));
    }
  };
  reactOptions.OnInstanceDestroyed =
      [weak = get_weak()](Mso::CntPtr<Mso::React::IReactContext> &&context, Mso::React::IReactInstance &) {
        if (auto strong = weak.get()) {
          strong->m_instanceDestroyedEvent(*strong, winrt::make<InstanceDestroyedEventArgs>(std::move(context)));
        }
      };

  std::string jsBundleFile = to_string(m_instanceSettings.JavaScriptBundleFile());
  std::string jsMainModuleName = to_string(m_instanceSettings.JavaScriptMainModuleName());
  if (jsBundleFile.empty()) {
    if (!jsMainModuleName.empty()) {
      jsBundleFile = jsMainModuleName;
    } else {
      jsBundleFile = "index.windows";
    }
  }

  reactOptions.Identity = jsBundleFile;
  return make<Mso::AsyncActionFutureAdapter>(m_reactHost->ReloadInstanceWithOptions(std::move(reactOptions)));
}

IAsyncAction ReactNativeHost::UnloadInstance() noexcept {
  return make<Mso::AsyncActionFutureAdapter>(m_reactHost->UnloadInstance());
}

Mso::React::IReactHost *ReactNativeHost::ReactHost() noexcept {
  return m_reactHost.Get();
}

winrt::event_token ReactNativeHost::InstanceCreated(
    Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceCreatedEventArgs> const
        &handler) noexcept {
  return m_instanceCreatedEvent.add(handler);
}

void ReactNativeHost::InstanceCreated(winrt::event_token const &token) noexcept {
  m_instanceCreatedEvent.remove(token);
}

winrt::event_token ReactNativeHost::InstanceLoaded(
    Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceLoadedEventArgs> const &handler) noexcept {
  return m_instanceLoadedEvent.add(handler);
}

void ReactNativeHost::InstanceLoaded(winrt::event_token const &token) noexcept {
  m_instanceLoadedEvent.remove(token);
}

winrt::event_token ReactNativeHost::InstanceDestroyed(
    Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::InstanceDestroyedEventArgs> const
        &handler) noexcept {
  return m_instanceDestroyedEvent.add(handler);
}

void ReactNativeHost::InstanceDestroyed(winrt::event_token const &token) noexcept {
  m_instanceDestroyedEvent.remove(token);
}

} // namespace winrt::Microsoft::ReactNative::implementation
