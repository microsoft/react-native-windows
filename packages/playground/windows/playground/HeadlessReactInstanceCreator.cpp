// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//

#include "pch.h"
#include "HeadlessReactInstanceCreator.h"
#include <ReactUWP/IXamlRootView.h>
#include <ReactUWP/ReactUwp.h>
#include "NativeModules/SampleNativeModuleProvider.h"

std::shared_ptr<react::uwp::IReactInstance>
HeadlessReactInstanceCreator::getInstance() {
  if (m_instance)
    return m_instance;

  std::shared_ptr<facebook::react::NativeModuleProvider> moduleProvider =
      std::make_shared<SampleNativeModuleProvider>();

  std::shared_ptr<react::uwp::IReactInstance> reactInstance =
      react::uwp::CreateReactInstance(moduleProvider, nullptr);

  react::uwp::ReactInstanceSettings settings;
  settings.UseLiveReload = true;
  settings.UseWebDebugger = true;

  reactInstance->Start(reactInstance, settings);
  reactInstance->loadBundle("Samples\\headlessjs");

  m_instance = reactInstance;
  return m_instance;
}

void HeadlessReactInstanceCreator::markAsNeedsReload() {
  m_instance->SetAsNeedsReload();
  m_instance = nullptr;
}

void HeadlessReactInstanceCreator::persistUseWebDebugger(
    bool /*useWebDebugger*/) {}

void HeadlessReactInstanceCreator::persistUseLiveReload(
    bool /*useLiveReload*/) {}

/* static */ std::shared_ptr<HeadlessReactInstanceCreator>
HeadlessReactInstanceCreator::get() {
  if (!s_headlessReactInstanceCreator) {
    s_headlessReactInstanceCreator =
        std::make_shared<HeadlessReactInstanceCreator>();
  }

  return s_headlessReactInstanceCreator;
}

std::shared_ptr<HeadlessReactInstanceCreator>
HeadlessReactInstanceCreator::s_headlessReactInstanceCreator = nullptr;
