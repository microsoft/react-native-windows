// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>
#include "ReactNativeHost.h"
#include "ReactNativeHost.g.cpp"

#include "ReactPackageBuilder.h"
#include "RedBox.h"

#include <winrt/Windows.Foundation.Collections.h>
#include "ReactInstanceSettings.h"

using namespace winrt;
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
  if (!m_packageProviders) {
    m_packageProviders = single_threaded_vector<IReactPackageProvider>();
  }

  return m_packageProviders;
}

void ReactNativeHost::PackageProviders(IVector<IReactPackageProvider> const &value) noexcept {
  m_packageProviders = value;
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

void ReactNativeHost::ReloadInstance() noexcept {
#ifndef CORE_ABI
  auto modulesProvider = std::make_shared<NativeModulesProvider>();

  auto viewManagersProvider = std::make_shared<ViewManagersProvider>();

  if (!m_packageBuilder) {
    m_packageBuilder = make<ReactPackageBuilder>(modulesProvider, viewManagersProvider);

    if (m_packageProviders) {
      for (auto const &packageProvider : m_packageProviders) {
        packageProvider.CreatePackage(m_packageBuilder);
      }
    }
  }

  std::shared_ptr<Mso::React::IRedBoxHandler> redBoxHandler =
      m_instanceSettings.RedBoxHandler() ?
      Mso::React::CreateRedBoxHandler(m_instanceSettings.RedBoxHandler()) :
      nullptr;
#endif

  std::string jsBundleFile = to_string(m_instanceSettings.JavaScriptBundleFile());
  std::string jsMainModuleName = to_string(m_instanceSettings.JavaScriptMainModuleName());
  if (jsBundleFile.empty()) {
    if (!jsMainModuleName.empty()) {
      jsBundleFile = jsMainModuleName;
    } else {
      jsBundleFile = "index.windows";
    }
  }

  Mso::React::ReactOptions reactOptions{};
  reactOptions.Properties = m_instanceSettings.Properties();
  reactOptions.DeveloperSettings.IsDevModeEnabled = m_instanceSettings.EnableDeveloperMenu();
  reactOptions.DeveloperSettings.SourceBundleName = to_string(m_instanceSettings.DebugBundlePath());
  reactOptions.DeveloperSettings.UseWebDebugger = m_instanceSettings.UseWebDebugger();
  reactOptions.DeveloperSettings.UseDirectDebugger = m_instanceSettings.UseDirectDebugger();
  reactOptions.DeveloperSettings.DebuggerBreakOnNextLine = m_instanceSettings.DebuggerBreakOnNextLine();
  reactOptions.DeveloperSettings.UseFastRefresh = m_instanceSettings.UseFastRefresh();
  reactOptions.DeveloperSettings.UseLiveReload = m_instanceSettings.UseLiveReload();
  reactOptions.EnableJITCompilation = m_instanceSettings.EnableJITCompilation();
  reactOptions.DeveloperSettings.DebugHost = to_string(m_instanceSettings.DebugHost());
  reactOptions.BundleRootPath = to_string(m_instanceSettings.BundleRootPath());
  reactOptions.DeveloperSettings.DebuggerPort = m_instanceSettings.DebuggerPort();
#ifndef CORE_ABI
  reactOptions.ModuleProvider = modulesProvider;
  reactOptions.RedBoxHandler = redBoxHandler;
  reactOptions.ViewManagerProvider = viewManagersProvider;
#endif
  reactOptions.Identity = jsBundleFile;

#ifndef CORE_ABI
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
  legacySettings.RedBoxHandler = redBoxHandler;
 
  reactOptions.LegacySettings = std::move(legacySettings);
#endif

  m_reactHost->ReloadInstanceWithOptions(std::move(reactOptions));
}

Mso::React::IReactHost *ReactNativeHost::ReactHost() noexcept {
  return m_reactHost.Get();
}

} // namespace winrt::Microsoft::ReactNative::implementation
