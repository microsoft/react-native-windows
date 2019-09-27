// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "NativeModulesProvider.h"
#include "ReactNativeHost.h"
#include "ViewManagersProvider.h"

#include <ReactUWP/ReactUwp.h>
#include <winrt/Microsoft.ReactNative.h>

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Microsoft::ReactNative::Bridge;

namespace winrt::Microsoft::ReactNative::implementation {
void InitReactNative();

struct ReactInstanceCreator : react::uwp::IReactInstanceCreator {
  ReactInstanceCreator(
      Microsoft::ReactNative::ReactInstanceSettings instanceSettings,
      std::string jsBundleFile,
      std::string jsMainModuleName,
      std::shared_ptr<NativeModulesProvider> modulesProvider,
      std::shared_ptr<ViewManagersProvider> viewManagersProvider);

  std::string m_jsMainModuleName{};
  std::string m_jsBundleFile{};
  std::shared_ptr<NativeModulesProvider> m_modulesProvider{nullptr};
  std::shared_ptr<ViewManagersProvider> m_viewManagersProvider{nullptr};

  std::shared_ptr<react::uwp::IReactInstance> getInstance();
  void markAsNeedsReload();

  // TODO: Remove this wart. AFAIK, it was only added for the
  // Universal.SampleApp It's here because of the abstract base class.  The
  // ReactInstanceSettings in this project has a UseWebDebugger property
  // that should be used instead as it automatically "persists" by virtue
  // of being attached to the ReactNativeHost.
  void persistUseWebDebugger(bool /*useWebDebugger*/){};
  // TODO: Remove too
  void persistUseLiveReload(bool /*useliveReload*/){};

 private:
  Microsoft::ReactNative::ReactInstanceSettings m_instanceSettings{};
  std::shared_ptr<react::uwp::IReactInstance> m_instance;
};
} // namespace winrt::Microsoft::ReactNative::implementation
