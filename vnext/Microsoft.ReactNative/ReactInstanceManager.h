#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ReactInstanceManager.g.h"

#include "IReactContext.h"
#include "LifecycleState.h"
#include "NativeModulesProvider.h"
#include "ReactInstanceSettings.h"
#include "ViewManagersProvider.h"

#include <ReactUWP/IReactInstance.h>
#include <ReactUWP/ReactUwp.h>

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactInstanceManager : ReactInstanceManagerT<ReactInstanceManager> {
  ReactInstanceManager() = default;
  ReactInstanceManager(
      ReactNative::ReactInstanceSettings const &instanceSettings,
      std::string const &jsBundleFile,
      std::string const &jsMainModuleName,
      Windows::Foundation::Collections::IVectorView<IReactPackageProvider> const &packageProviders,
      bool useDeveloperSupport,
      LifecycleState const &initialLifecycleState);

  IAsyncOperation<IReactContext> GetOrCreateReactContextAsync();
  IReactContext CurrentReactContext() {
    return m_currentReactContext;
  }

  // TODO: ReactInstanceManager should be generating ReactContext objects
  // where the ReactContext references the ReactInstance (which would
  // reference the real implementation.
  std::shared_ptr<react::uwp::IReactInstanceCreator> InstanceCreator();
  std::shared_ptr<react::uwp::IReactInstance> Instance() {
    return InstanceCreator()->getInstance();
  }

  void OnSuspend();
  void OnEnteredBackground();
  void OnLeavingBackground();
  void OnResume(OnResumeAction const &action);

  void OnBackPressed();

 private:
  IReactContext m_currentReactContext{nullptr};
  ReactNative::ReactInstanceSettings m_instanceSettings{nullptr};
  std::string m_jsBundleFile{};
  std::string m_jsMainModuleName{};
  std::vector<IReactPackageProvider> m_packageProviders;
  bool m_useDeveloperSupport{false};
  std::shared_ptr<NativeModulesProvider> m_modulesProvider{nullptr};
  std::shared_ptr<ViewManagersProvider> m_viewManagersProvider{nullptr};
  IReactPackageBuilder m_packageBuilder;

  //	There should be one react instance creator per instance, as it
  //	both holds the current instance and is responsible for creating new
  //	instances on live reload.
  std::shared_ptr<react::uwp::IReactInstanceCreator> m_reactInstanceCreator{nullptr};

  IAsyncOperation<IReactContext> CreateReactContextCoreAsync();
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactInstanceManager : ReactInstanceManagerT<ReactInstanceManager, implementation::ReactInstanceManager> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
