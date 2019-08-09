// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactInstanceManager.h"
#include "ReactInstanceManager.g.cpp"

#include "ReactInstanceCreator.h"

#include <cxxreact/CxxModule.h>
#include <cxxreact/Instance.h>
#include <cxxreact/ModuleRegistry.h>

using namespace winrt;
using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Microsoft::ReactNative::Bridge;
using namespace winrt::Windows::Foundation::Collections;

namespace winrt::Microsoft::ReactNative::implementation {
ReactInstanceManager::ReactInstanceManager(
    Microsoft::ReactNative::ReactInstanceSettings instanceSettings,
    std::string jsBundleFile,
    std::string jsMainModuleName,
    IVectorView<IReactPackage> &packages,
    bool useDeveloperSupport,
    /*TODO*/ LifecycleState initialLifecycleState)
    : m_instanceSettings(instanceSettings),
      m_jsBundleFile(jsBundleFile),
      m_jsMainModuleName(jsMainModuleName),
      m_packages(packages),
      m_useDeveloperSupport(useDeveloperSupport) {
  if (packages == nullptr) {
    throw hresult_invalid_argument(L"packages");
  }

  // TODO: Create a LifeCycleStateMachine to raise events in response
  // to events from the application and associate it with the ReactContext.
  // Define the ILifecycleEventListener and add support to ReactContext to
  // register a lifecycle listener. Define the IBackgroundEventListener and add
  // support to ReactContext to register modules as background event listeners.
}

void ReactInstanceManager::OnSuspend() {
  OutputDebugStringW(L"TODO: ReactInstanceManager::OnSuspend not implemented");

  // DispatcherHelpers.AssertOnDispatcher();

  //_defaultBackButtonHandler = null;
  //_suspendCancellation?.Dispose();

  // if (_useDeveloperSupport)
  //{
  //    _devSupportManager.IsEnabled = false;
  //}

  //_lifecycleStateMachine.OnSuspend();
}

// Called when the host entered background mode.
void ReactInstanceManager::OnEnteredBackground() {
  OutputDebugStringW(
      L"TODO: ReactInstanceManager::OnEnteredBackground not implemented");

  // DispatcherHelpers.AssertOnDispatcher();
  //_lifecycleStateMachine.OnEnteredBackground();
}

// Called when the host is leaving background mode.
void ReactInstanceManager::OnLeavingBackground() {
  OutputDebugStringW(
      L"TODO: ReactInstanceManager::OnLeavingBackground not implemented");

  // DispatcherHelpers.AssertOnDispatcher();
  //_lifecycleStateMachine.OnLeavingBackground();
}

// Used when the application resumes to reset the back button handling in
// JavaScript.
void ReactInstanceManager::OnResume(OnResumeAction const & /*action*/) {
  OutputDebugStringW(L"TODO: ReactInstanceManager::OnResume not implemented");

  // see the ReactInstanceManager.cs from the C# implementation

  //_defaultBackButtonHandler = onBackPressed;
  //_suspendCancellation = new CancellationDisposable();

  // if (_useDeveloperSupport)
  //{
  //	_devSupportManager.IsEnabled = true;
  //}

  //_lifecycleStateMachine.OnResume();
}

void ReactInstanceManager::OnBackPressed() {
  throw hresult_not_implemented(
      L"TODO: ReactInstanceManager::OnBackPressed not implemented");

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

std::shared_ptr<react::uwp::IReactInstanceCreator>
ReactInstanceManager::InstanceCreator() {
  if (m_reactInstanceCreator == nullptr) {
    m_reactInstanceCreator = std::make_shared<ReactInstanceCreator>(
        m_instanceSettings,
        m_jsBundleFile,
        m_jsMainModuleName,
        m_modulesProvider,
        m_viewManagersProvider);
  }

  return m_reactInstanceCreator;
}

auto ReactInstanceManager::GetOrCreateReactContextAsync()
    -> IAsyncOperation<ReactContext> {
  if (m_currentReactContext != nullptr)
    co_return m_currentReactContext;

  m_currentReactContext = co_await CreateReactContextCoreAsync();

  co_return m_currentReactContext;
}

// TODO: Should we make this method async?  On first run when getInstance
// is called it starts things up. Does this need to block?
auto ReactInstanceManager::CreateReactContextCoreAsync()
    -> IAsyncOperation<ReactContext> {
  auto reactContext = ReactContext();

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

  auto moduleRegistryList = single_threaded_vector<NativeModuleBase>();
  if (m_modulesProvider == nullptr) {
    m_modulesProvider = std::make_shared<NativeModulesProvider>();

    // TODO: Define a CoreModulesPackage, load it here.
    // TODO: Wrap/re-implement our existing set of core modules and add
    // them to the CoreModulesPackage.

    for (auto package : m_packages) {
      auto modules = package.CreateNativeModules(reactContext);
      for (auto module : modules) {
        // TODO: Allow a module to override another if they conflict on name?
        // Something that the registry would handle.  And should that inform
        // which modules get iniitalized?
        m_modulesProvider->RegisterModule(module);
        moduleRegistryList.Append(module);
      }
    }
  }

  if (m_viewManagersProvider == nullptr) {
    m_viewManagersProvider = std::make_shared<ViewManagersProvider>();

    // TODO: Register default modules
    // The registration that currently happens in the moduleregistry.cpp
    // should happen here if we convert all modules to go through the ABI
    // rather than directly against facebook's types.
  }

  // TODO: Could access to the module registry be easier if the ReactInstance
  // implementation were lifted up into this project.

  auto instancePtr = InstanceCreator()->getInstance();
  auto reactInstance = winrt::make<Bridge::implementation::ReactInstance>(
      instancePtr, moduleRegistryList.GetView());

  Bridge::implementation::ReactContext *contextImpl{
      get_self<Bridge::implementation::ReactContext>(reactContext)};
  contextImpl->InitializeWithInstance(reactInstance);

  // TODO: Investigate whether we need the equivalent of the
  // LimitedConcurrencyActionQueue from the C# implementation that is used to
  // sequence actions in order and ensure async continuations are performed on
  // the same thread.  It's used as the type of queue for native modules.  It
  // would be created before the instance and set as its queue configuration.
  // It's then used by the instance internally as part of this InitializeAsync.

  co_await reactInstance.InitializeAsync();

  co_return reactContext;
}
} // namespace winrt::Microsoft::ReactNative::implementation
