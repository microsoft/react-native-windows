// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactNativeHost.h"
#include "ReactNativeHost.g.cpp"

#include "ReactPackageBuilder.h"
#include "RedBox.h"

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

  if (!m_packageBuilder) {
    m_packageBuilder = make<ReactPackageBuilder>(modulesProvider, viewManagersProvider);

    if (auto packageProviders = InstanceSettings().PackageProviders()) {
      for (auto const &packageProvider : packageProviders) {
        packageProvider.CreatePackage(m_packageBuilder);
      }
    }
  }

  react::uwp::ReactInstanceSettings legacySettings{};
  legacySettings.BundleRootPath = to_string(m_instanceSettings.BundleRootPath());
  legacySettings.ByteCodeFileUri = to_string(m_instanceSettings.ByteCodeFileUri());
  legacySettings.DebugBundlePath = to_string(m_instanceSettings.DebugBundlePath());
  legacySettings.DebugHost = to_string(m_instanceSettings.DebugHost());
  legacySettings.EnableByteCodeCaching = m_instanceSettings.EnableByteCodeCaching();
  legacySettings.EnableDeveloperMenu = m_instanceSettings.EnableDeveloperMenu();
  legacySettings.EnableJITCompilation = m_instanceSettings.EnableJITCompilation();
  legacySettings.UseDirectDebugger = m_instanceSettings.UseDirectDebugger();
  legacySettings.DebuggerBreakOnNextLine = m_instanceSettings.DebuggerBreakOnNextLine();
  legacySettings.UseJsi = m_instanceSettings.UseJsi();
  legacySettings.UseFastRefresh = m_instanceSettings.UseFastRefresh();
  legacySettings.UseLiveReload = m_instanceSettings.UseLiveReload();
  legacySettings.UseWebDebugger = m_instanceSettings.UseWebDebugger();
  legacySettings.DebuggerPort = m_instanceSettings.DebuggerPort();
  legacySettings.SourceBundleHost = to_string(m_instanceSettings.SourceBundleHost());
  legacySettings.SourceBundlePort = m_instanceSettings.SourceBundlePort();

  if (m_instanceSettings.RedBoxHandler()) {
    legacySettings.RedBoxHandler = std::move(Mso::React::CreateRedBoxHandler(m_instanceSettings.RedBoxHandler()));
  }

  Mso::React::ReactOptions reactOptions{};
  reactOptions.Properties = m_instanceSettings.Properties();
  reactOptions.Notifications = m_instanceSettings.Notifications();
  reactOptions.DeveloperSettings.IsDevModeEnabled = legacySettings.EnableDeveloperMenu;
  reactOptions.DeveloperSettings.SourceBundleName = legacySettings.DebugBundlePath;
  reactOptions.DeveloperSettings.UseWebDebugger = legacySettings.UseWebDebugger;
  reactOptions.DeveloperSettings.UseDirectDebugger = legacySettings.UseDirectDebugger;
  reactOptions.DeveloperSettings.DebuggerBreakOnNextLine = legacySettings.DebuggerBreakOnNextLine;
  reactOptions.DeveloperSettings.UseFastRefresh = legacySettings.UseFastRefresh;
  reactOptions.DeveloperSettings.UseLiveReload = legacySettings.UseLiveReload;
  reactOptions.EnableJITCompilation = legacySettings.EnableJITCompilation;
  reactOptions.DeveloperSettings.DebugHost = legacySettings.DebugHost;
  reactOptions.BundleRootPath = legacySettings.BundleRootPath;
  reactOptions.DeveloperSettings.DebuggerPort = legacySettings.DebuggerPort;
  reactOptions.RedBoxHandler = legacySettings.RedBoxHandler;
  reactOptions.DeveloperSettings.SourceBundleHost = legacySettings.SourceBundleHost;
  reactOptions.DeveloperSettings.SourceBundlePort =
      legacySettings.SourceBundlePort != 0 ? std::to_string(legacySettings.SourceBundlePort) : "";

  reactOptions.LegacySettings = std::move(legacySettings);

  reactOptions.ModuleProvider = modulesProvider;
  reactOptions.ViewManagerProvider = viewManagersProvider;

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
