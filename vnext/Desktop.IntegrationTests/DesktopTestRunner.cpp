// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <TestRunner.h>

#include <CreateModules.h>
#include <IUIManager.h>
#include <Modules/NetworkingModule.h>
#include <Modules/WebSocketModule.h>
#include <NativeModuleFactories.h>
#include <Threading/MessageQueueThreadFactory.h>
#include "ChakraRuntimeHolder.h"
#include "DesktopTestInstance.h"
#include "TestModule.h"
#include "TestRootView.h"

using namespace facebook::react;
using namespace facebook::xplat::module;

using std::shared_ptr;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;

namespace Microsoft::React::Test {

shared_ptr<ITestInstance> TestRunner::GetInstance(
    string &&jsBundleFile,
    vector<tuple<string, CxxModule::Provider>> &&cxxModules,
    shared_ptr<DevSettings> devSettings) noexcept {
  vector<unique_ptr<IViewManager>> viewManagers;
  viewManagers.push_back(unique_ptr<IViewManager>(new TestViewManager("RCTView")));
  viewManagers.push_back(unique_ptr<IViewManager>(new TestViewManager("RCTText")));
  viewManagers.push_back(unique_ptr<IViewManager>(new TestViewManager("RCTRawText")));
  viewManagers.push_back(unique_ptr<IViewManager>(new TestViewManager("RCTScrollView")));

  auto uiManager = createIUIManager(std::move(viewManagers), new TestNativeUIManager());
  auto nativeQueue = react::uwp::MakeJSQueueThread();
  auto jsQueue = react::uwp::MakeJSQueueThread();

  devSettings->jsiRuntimeHolder = std::make_shared<ChakraRuntimeHolder>(devSettings, jsQueue, nullptr, nullptr);

  vector<tuple<string, CxxModule::Provider, shared_ptr<MessageQueueThread>>> extraModules{
      std::make_tuple(
          "AsyncLocalStorage",
          []() -> unique_ptr<CxxModule> { return CreateAsyncStorageModule(L"ReactNativeAsyncStorage"); },
          nativeQueue),
      std::make_tuple(
          "WebSocketModule",
          []() -> unique_ptr<CxxModule> { return std::make_unique<WebSocketModule>(); },
          nativeQueue),
      std::make_tuple(
          "Networking",
          []() -> unique_ptr<CxxModule> { return std::make_unique<Microsoft::React::NetworkingModule>(); },
          nativeQueue),
      std::make_tuple(
          "Timing", [nativeQueue]() -> unique_ptr<CxxModule> { return CreateTimingModule(nativeQueue); }, nativeQueue),
      // Apparently mandatory for /IntegrationTests
      std::make_tuple(
          TestAppStateModule::name,
          []() -> unique_ptr<CxxModule> { return std::make_unique<TestAppStateModule>(); },
          nativeQueue),
      // Apparently mandatory for /IntegrationTests
      std::make_tuple(
          "UIManager",
          [uiManager]() -> unique_ptr<CxxModule> { return createUIManagerModule(uiManager); },
          nativeQueue),
      // Apparently mandatory for /IntegrationTests
      std::make_tuple(
          TestDeviceInfoModule::name,
          []() -> unique_ptr<CxxModule> { return std::make_unique<TestDeviceInfoModule>(); },
          nativeQueue)};

  // <0> string
  // <1> CxxModule::Provider
  for (auto &t : cxxModules) {
    extraModules.emplace_back(std::get<0>(t), std::get<1>(t), nativeQueue);
  }

  // Update settings.
  devSettings->platformName = "windesktop";

  auto instanceWrapper = CreateReactInstance(
      "",
      std::move(extraModules),
      nullptr,
      std::move(uiManager),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings));
  instanceWrapper->loadBundle(std::move(jsBundleFile));

  return shared_ptr<ITestInstance>(new DesktopTestInstance(std::move(instanceWrapper)));
}

} // namespace Microsoft::React::Test
