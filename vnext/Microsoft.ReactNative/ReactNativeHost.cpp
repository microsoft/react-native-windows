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
#include "ReactInstanceSettings.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;

#ifndef CORE_ABI
using namespace xaml;
using namespace xaml::Controls;
#endif

namespace winrt::Microsoft::ReactNative::implementation {

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

IAsyncAction ReactNativeHost::ReloadInstance() noexcept {
#ifndef CORE_ABI
  auto modulesProvider = std::make_shared<NativeModulesProvider>();
  auto viewManagersProvider = std::make_shared<ViewManagersProvider>();
  auto turboModulesProvider = std::make_shared<TurboModulesProvider>();

  if (!m_packageBuilder) {
    m_packageBuilder = make<ReactPackageBuilder>(modulesProvider, viewManagersProvider, turboModulesProvider);

    if (auto packageProviders = InstanceSettings().PackageProviders()) {
      for (auto const &packageProvider : packageProviders) {
        packageProvider.CreatePackage(m_packageBuilder);
      }
    }
  }

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
  reactOptions.DeveloperSettings.SourceBundlePort =
      m_instanceSettings.SourceBundlePort() != 0 ? std::to_string(m_instanceSettings.SourceBundlePort()) : "";

  reactOptions.ByteCodeFileUri = to_string(m_instanceSettings.ByteCodeFileUri());
  reactOptions.EnableByteCodeCaching = m_instanceSettings.EnableByteCodeCaching();
  reactOptions.UseJsi = m_instanceSettings.UseJsi();

  reactOptions.ModuleProvider = modulesProvider;
  reactOptions.ViewManagerProvider = viewManagersProvider;
  reactOptions.TurboModuleProvider = turboModulesProvider;

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
#else
  // Core ABI work needed
  VerifyElseCrash(false);
#endif
}

IAsyncAction ReactNativeHost::UnloadInstance() noexcept {
  return make<Mso::AsyncActionFutureAdapter>(m_reactHost->UnloadInstance());
}

Mso::React::IReactHost *ReactNativeHost::ReactHost() noexcept {
  return m_reactHost.Get();
}

} // namespace winrt::Microsoft::ReactNative::implementation
