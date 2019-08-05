// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactInstanceCreator.h"
#include <ReactUWP/ReactUwp.h>

using namespace winrt;

namespace winrt::Microsoft::ReactNative::implementation {

void InitReactNative() {
#if _DEBUG
  facebook::react::InitializeLogging(
      [](facebook::react::RCTLogLevel /*logLevel*/, const char *message) {
        std::string str = std::string("ReactNative:") + message;
        OutputDebugStringA(str.c_str());
      });
#endif
}

ReactInstanceCreator::ReactInstanceCreator(
    Microsoft::ReactNative::ReactInstanceSettings instanceSettings,
    std::string jsBundleFile,
    std::string jsMainModuleName,
    std::shared_ptr<NativeModulesProvider> modulesProvider,
    std::shared_ptr<ViewManagersProvider> viewManagersProvider)
    : m_instanceSettings(instanceSettings),
      m_jsBundleFile(jsBundleFile),
      m_jsMainModuleName(jsMainModuleName),
      m_modulesProvider(modulesProvider),
      m_viewManagersProvider(viewManagersProvider) {
  if (instanceSettings == nullptr) {
    throw hresult_null_argument(L"instanceSettings");
  }

  if (modulesProvider == nullptr) {
    throw hresult_null_argument(L"modulesProvider");
  }

  if (viewManagersProvider == nullptr) {
    throw hresult_null_argument(L"viewManagersProvider");
  }
}

std::shared_ptr<react::uwp::IReactInstance>
ReactInstanceCreator::getInstance() {
  if (m_instance)
    return m_instance;

  std::shared_ptr<react::uwp::IReactInstance> reactInstance =
      react::uwp::CreateReactInstance(
          m_modulesProvider, m_viewManagersProvider);

  react::uwp::ReactInstanceSettings settings;
  settings.BundleRootPath = to_string(m_instanceSettings.BundleRootPath());
  settings.ByteCodeFileUri = to_string(m_instanceSettings.ByteCodeFileUri());
  settings.DebugBundlePath = to_string(m_instanceSettings.DebugBundlePath());
  settings.DebugHost = to_string(m_instanceSettings.DebugHost());
  settings.EnableByteCodeCacheing = m_instanceSettings.EnableByteCodeCaching();
  settings.EnableJITCompilation = m_instanceSettings.EnableJITCompilation();
  settings.UseDirectDebugger = m_instanceSettings.UseDirectDebugger();
  settings.UseJsi = m_instanceSettings.UseJsi();
  settings.UseLiveReload = m_instanceSettings.UseLiveReload();
  settings.UseWebDebugger = m_instanceSettings.UseWebDebugger();

  reactInstance->Start(reactInstance, settings);

  if (m_jsBundleFile.empty()) {
    if (!m_jsMainModuleName.empty()) {
      m_jsBundleFile = m_jsMainModuleName;
    } else {
      m_jsBundleFile = "App.windows";
    }
  }

  reactInstance->loadBundle(std::move(m_jsBundleFile));

  m_instance = reactInstance;

  InitReactNative();

  return m_instance;
}

void ReactInstanceCreator::markAsNeedsReload() {
  m_instance->SetAsNeedsReload();
  m_instance = nullptr;
}


void persistUseWebDebugger(bool useWebDebugger) {

}
} // namespace winrt::Microsoft::ReactNative::implementation
