// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "NativeModulesProvider.h"
#include "ReactNativeHost.h"
#include "ViewManagersProvider.h"

#include <ReactUWP/ReactUwp.h>
#include <winrt/Microsoft.ReactNative.h>

namespace winrt::Microsoft::ReactNative::implementation {

void InitReactNative();

struct ReactInstanceCreator : react::uwp::IReactInstanceCreator {
  ReactInstanceCreator(
      ReactNative::ReactInstanceSettings const &instanceSettings,
      std::string const &jsBundleFile,
      std::string const &jsMainModuleName,
      std::shared_ptr<NativeModulesProvider> const &modulesProvider,
      std::shared_ptr<ViewManagersProvider> const &viewManagersProvider);

  std::string m_jsMainModuleName{};
  std::string m_jsBundleFile{};
  std::shared_ptr<NativeModulesProvider> m_modulesProvider{nullptr};
  std::shared_ptr<ViewManagersProvider> m_viewManagersProvider{nullptr};

  std::shared_ptr<react::uwp::IReactInstance> getInstance();
  void markAsNeedsReload();

  void persistUseWebDebugger(bool useWebDebugger) {
    m_instanceSettings.UseWebDebugger(useWebDebugger);
  };

  void persistUseLiveReload(bool useliveReload) {
    m_instanceSettings.UseLiveReload(useliveReload);
  };

 private:
  ReactNative::ReactInstanceSettings m_instanceSettings{};
  std::shared_ptr<react::uwp::IReactInstance> m_instance;
};
} // namespace winrt::Microsoft::ReactNative::implementation
