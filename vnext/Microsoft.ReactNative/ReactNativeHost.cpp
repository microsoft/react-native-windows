// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactNativeHost.h"
#include "ReactNativeHost.g.cpp"

#include "ReactPackageBuilder.h"

using namespace winrt;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;

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

  Mso::React::ReactOptions reactOptions{};
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

  reactOptions.UserData = m_instanceSettings.UserData();

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

  m_reactHost->ReloadInstanceWithOptions(std::move(reactOptions));
}

Mso::React::IReactHost *ReactNativeHost::ReactHost() noexcept {
  return m_reactHost.Get();
}

// TODO: Create a LifeCycleStateMachine in constructor to raise events in response
// to events from the application and associate it with the ReactContext.
// Define the ILifecycleEventListener and add support to ReactContext to
// register a lifecycle listener. Define the IBackgroundEventListener and add
// support to ReactContext to register modules as background event listeners.

void ReactNativeHost::OnSuspend() noexcept {
  OutputDebugStringW(L"TODO: ReactNativeHost::OnSuspend not implemented");

  // DispatcherHelpers.AssertOnDispatcher();

  //_defaultBackButtonHandler = null;
  //_suspendCancellation?.Dispose();

  // if (_useDeveloperSupport)
  //{
  //    _devSupportManager.IsEnabled = false;
  //}

  //_lifecycleStateMachine.OnSuspend();
}

void ReactNativeHost::OnEnteredBackground() noexcept {
  OutputDebugStringW(L"TODO: ReactNativeHost::OnEnteredBackground not implemented");

  // DispatcherHelpers.AssertOnDispatcher();
  //_lifecycleStateMachine.OnEnteredBackground();
}

void ReactNativeHost::OnLeavingBackground() noexcept {
  OutputDebugStringW(L"TODO: ReactNativeHost::OnLeavingBackground not implemented");

  // DispatcherHelpers.AssertOnDispatcher();
  //_lifecycleStateMachine.OnLeavingBackground();
}

void ReactNativeHost::OnResume(OnResumeAction const & /*action*/) noexcept {
  OutputDebugStringW(L"TODO: ReactNativeHost::OnResume not implemented");

  // see the ReactInstanceManager.cs from the C# implementation

  //_defaultBackButtonHandler = onBackPressed;
  //_suspendCancellation = new CancellationDisposable();

  // if (_useDeveloperSupport)
  //{
  //	_devSupportManager.IsEnabled = true;
  //}

  //_lifecycleStateMachine.OnResume();
}

void ReactNativeHost::OnBackPressed() noexcept {
  OutputDebugStringW(L"TODO: ReactNativeHost::OnBackPressed not implemented");

  // DispatcherHelpers.AssertOnDispatcher();
  // var reactContext = _currentReactContext;
  // if (reactContext == null)
  //{
  //	RnLog.Warn(ReactConstants.RNW, $"ReactInstanceManager: OnBackPressed:
  // Instance detached from instance manager.");
  // InvokeDefaultOnBackPressed();
  //}
  // else
  //{
  //	reactContext.GetNativeModule<DeviceEventManagerModule>().EmitHardwareBackPressed();
  //}
}

} // namespace winrt::Microsoft::ReactNative::implementation
