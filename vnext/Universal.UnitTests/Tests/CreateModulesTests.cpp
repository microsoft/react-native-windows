// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include <CppUnitTest.h>

#include <Modules/AppStateModule.h>
#include <Modules/DevSupportManagerUwp.h>
#include <Modules/DeviceInfoModule.h>
#include <Modules/LocationObserverModule.h>
#include <Modules/NativeUIManager.h>
#include <Modules/TimingModule.h>
#include <Modules/UIManagerModule.h>
#include <Modules/WebSocketModuleUwp.h>
#include <ReactUWP/CreateUwpModules.h>
#include <ReactUWP/IReactInstance.h>
#include <ReactUWP/InstanceFactory.h>
#include <ViewManager.h>
#include <cxxreact/MessageQueueThread.h>

#include <CreateModules.h>

using namespace facebook::react;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace react::uwp;

TEST_CLASS(CreateModulesTest){TEST_METHOD(CreateModules_AppStateModule){
    auto appStateModule =
        std::make_unique<AppStateModule>(std::make_unique<AppState>());

Assert::IsFalse(appStateModule == nullptr);
}

TEST_METHOD(CreateModules_DevSupportManager) {
  auto devSupportManager = std::make_shared<DevSupportManager>();
  Assert::IsFalse(devSupportManager == nullptr);
}

TEST_METHOD(CreateModules_DeviceInfoModule) {
  auto deviceInfoModule =
      std::make_unique<DeviceInfoModule>(std::make_unique<DeviceInfo>());

  Assert::IsFalse(deviceInfoModule == nullptr);
}

TEST_METHOD(CreateModules_LocationObserverModule) {
  auto messageQueueThread =
      std::shared_ptr<facebook::react::MessageQueueThread>(nullptr);

  auto locationObserverModule =
      std::make_unique<LocationObserverModule>(messageQueueThread);

  Assert::IsFalse(locationObserverModule == nullptr);
}

TEST_METHOD(CreateModules_NativeUIManager) {
  auto nativeUIManager = std::make_unique<react::uwp::NativeUIManager>();

  Assert::IsFalse(nativeUIManager == nullptr);
}

TEST_METHOD(CreateModules_TimingModule) {
  auto messageQueueThread =
      std::shared_ptr<facebook::react::MessageQueueThread>(nullptr);

  auto timingModule = CreateTimingModule(messageQueueThread);

  Assert::IsFalse(timingModule == nullptr);
}

TEST_METHOD(CreateModules_UIManagerModule) {
  auto reactInstance = react::uwp::CreateReactInstance(nullptr);
  Assert::IsFalse(reactInstance == nullptr);

  auto uiManager = CreateUIManager(reactInstance, nullptr);
  Assert::IsFalse(uiManager == nullptr);

  auto uiManagerModule = createUIManagerModule(std::move(uiManager));
  Assert::IsFalse(uiManagerModule == nullptr);
}

TEST_METHOD(CreateModules_WebSocketModule) {
  auto webSocketModule = std::make_unique<WebSocketModule>();

  Assert::IsFalse(webSocketModule == nullptr);
}
}
;
