// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactNativeHost.h"
#include "ReactNativeHost.g.cpp"

#include "IReactContext.h"
#include "ReactHost/AsyncFuture.h"
#include "ReactInstanceSettings.h"
#include "ReactPackageBuilder.h"
#include "ReactRootView2.h"
#include "ReactSupport.h"

#include <NativeModuleProvider.h>
#include <ViewManager.h>
#include <ViewManagerProvider.h>

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

inline ReactNative::IReactContext ReactNativeHost::CurrentReactContext() noexcept {
  return m_currentReactContext;
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

IAsyncAction ReactNativeHost::ReloadInstance() noexcept {
  // return Mso::FutureToAsyncAction(m_reactHost->ReloadInstance());

  if (m_currentReactContext != nullptr)
    co_return;

  /* TODO hook up an exception handler if UseDeveloperSupport is set
if (m_useDeveloperSupport) {
if (m_nativeModuleCallExceptionHandler) {
  reactContext.NativeModuleCallExceptionHandler =
      m_nativeModuleCallExceptionHandler;
} else {
  reactContext.NativeModuleCallExceptionHandler =
    m_devSupportManager.HandleException;
}
}
*/

  if (m_modulesProvider == nullptr) {
    m_modulesProvider = std::make_shared<NativeModulesProvider>();

    // TODO: Define a CoreModulesPackage, load it here.
    // TODO: Wrap/re-implement our existing set of core modules and add
    // them to the CoreModulesPackage.
  }

  if (m_viewManagersProvider == nullptr) {
    m_viewManagersProvider = std::make_shared<ViewManagersProvider>();

    // TODO: Register default modules
    // The registration that currently happens in the moduleregistry.cpp
    // should happen here if we convert all modules to go through the ABI
    // rather than directly against facebook's types.
  }

  if (!m_packageBuilder) {
    m_packageBuilder = make<ReactPackageBuilder>(m_modulesProvider, m_viewManagersProvider);

    for (auto const &packageProvider : m_packageProviders) {
      packageProvider.CreatePackage(m_packageBuilder);
    }
  }

  // TODO: Could access to the module registry be easier if the ReactInstance
  // implementation were lifted up into this project.

  auto reactInstance = Instance();

  m_currentReactContext = winrt::make<ReactContext>(reactInstance).as<IReactContext>();

  // TODO: Investigate whether we need the equivalent of the
  // LimitedConcurrencyActionQueue from the C# implementation that is used to
  // sequence actions in order and ensure async continuations are performed on
  // the same thread.  It's used as the type of queue for native modules.  It
  // would be created before the instance and set as its queue configuration.
  // It's then used by the instance internally as part of this InitializeAsync.

  co_return;
}

std::shared_ptr<react::uwp::IReactInstance> ReactNativeHost::Instance() noexcept {
  if (m_instance)
    return m_instance;

  std::shared_ptr<react::uwp::IReactInstance> reactInstance =
      react::uwp::CreateReactInstance(m_modulesProvider, m_viewManagersProvider);

  react::uwp::ReactInstanceSettings settings;
  settings.BundleRootPath = to_string(m_instanceSettings.BundleRootPath());
  settings.ByteCodeFileUri = to_string(m_instanceSettings.ByteCodeFileUri());
  settings.DebugBundlePath = to_string(m_instanceSettings.DebugBundlePath());
  settings.DebugHost = to_string(m_instanceSettings.DebugHost());
  settings.EnableByteCodeCaching = m_instanceSettings.EnableByteCodeCaching();
  settings.EnableDeveloperMenu = m_instanceSettings.EnableDeveloperMenu();
  settings.EnableJITCompilation = m_instanceSettings.EnableJITCompilation();
  settings.UseDirectDebugger = m_instanceSettings.UseDirectDebugger();
  settings.UseJsi = m_instanceSettings.UseJsi();
  settings.UseLiveReload = m_instanceSettings.UseLiveReload();
  settings.UseWebDebugger = m_instanceSettings.UseWebDebugger();

  reactInstance->Start(reactInstance, settings);

  std::string jsBundleFile = to_string(m_instanceSettings.JavaScriptBundleFile());
  std::string jsMainModuleName = to_string(m_instanceSettings.JavaScriptMainModuleName());
  if (jsBundleFile.empty()) {
    if (!jsMainModuleName.empty()) {
      jsBundleFile = jsMainModuleName;
    } else {
      jsBundleFile = "index.windows";
    }
  }

  reactInstance->loadBundle(std::move(jsBundleFile));

  m_instance = reactInstance;

  //TODO: InitReactNative();

  return m_instance;
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

void ReactNativeHost::OnResume(OnResumeAction const &action) noexcept {
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
