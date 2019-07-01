// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <TestRunner.h>

#include <Modules/AppStateModule.h>
#include <CreateModules.h>
#include <Modules/NetworkingModule.h>
#include <IUIManager.h>
#include <NativeModuleFactories.h>
#include <WebSocketModule.h>
#include "DesktopTestInstance.h"
#include "TestMessageQueueThread.h"
#include "TestModule.h"
#include "TestRootView.h"
#include "ChakraJSIRuntimeHolder.h"

using namespace facebook::react;
using namespace facebook::xplat::module;

using std::make_tuple;
using std::make_unique;
using std::make_shared;
using std::move;
using std::shared_ptr;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;

namespace Microsoft::React::Test
{

shared_ptr<ITestInstance> TestRunner::GetInstance(
  string&& jsBundleFile,
  vector<tuple<string, CxxModule::Provider>>&& cxxModules,
  shared_ptr<DevSettings> devSettings) noexcept
{
  vector<unique_ptr<IViewManager>> viewManagers;
  viewManagers.push_back(unique_ptr<IViewManager>(new TestViewManager("RCTView")));
  viewManagers.push_back(unique_ptr<IViewManager>(new TestViewManager("RCTText")));
  viewManagers.push_back(unique_ptr<IViewManager>(new TestViewManager("RCTRawText")));
  viewManagers.push_back(unique_ptr<IViewManager>(new TestViewManager("RCTScrollView")));

  auto uiManager = createIUIManager(move(viewManagers), new TestNativeUIManager());
  auto nativeQueue = make_shared<TestMessageQueueThread>();
  auto jsQueue = make_shared<TestMessageQueueThread>();

  devSettings->jsiRuntimeHolder = std::make_shared<ChakraJSIRuntimeHolder>(devSettings, jsQueue, nullptr, nullptr);

  vector<tuple<string, CxxModule::Provider, shared_ptr<MessageQueueThread>>> extraModules
  {
    make_tuple(
      "AsyncLocalStorage",
      []() -> unique_ptr<CxxModule> { return CreateAsyncStorageModule(L"ReactNativeAsyncStorage"); },
      nativeQueue
    ),
    make_tuple(
      "WebSocketModule",
      []() -> unique_ptr<CxxModule> { return make_unique<WebSocketModule>(); },
      nativeQueue
    ),
    make_tuple(
      "Networking",
      []() -> unique_ptr<CxxModule> { return make_unique<Microsoft::React::NetworkingModule>(); },
      nativeQueue
    ),
    make_tuple(
      "Timing",
      [nativeQueue]() -> unique_ptr<CxxModule> { return CreateTimingModule(nativeQueue); },
      nativeQueue
    ),
    // Apparently mandatory for /IntegrationTests
    make_tuple(
      AppStateModule::name,
      []() -> unique_ptr<CxxModule> { return make_unique<AppStateModule>(make_unique<AppState>()); },
      nativeQueue
    ),
    // Apparently mandatory for /IntegrationTests
    make_tuple(
      "UIManager",
      [uiManager]() -> unique_ptr<CxxModule>
      {
        return createUIManagerModule(uiManager);
      },
      nativeQueue
    ),
    // Apparently mandatory for /IntegrationTests
    make_tuple(
      TestDeviceInfoModule::name,
      []() -> unique_ptr<CxxModule> { return make_unique<TestDeviceInfoModule>(); },
      nativeQueue
    )
  };

  // <0> string
  // <1> CxxModule::Provider
  for (auto& t : cxxModules)
  {
    extraModules.emplace_back(std::get<0>(t), std::get<1>(t), nativeQueue);
  }

  // Update settings.
  devSettings->platformName = "windesktop";

  auto instanceWrapper = CreateReactInstance(
    "",
    std::move(extraModules),
    std::move(uiManager),
    std::move(jsQueue),
    std::move(nativeQueue),
    std::move(devSettings));
  instanceWrapper->loadBundle(std::move(jsBundleFile));

  return shared_ptr<ITestInstance>(new DesktopTestInstance(move(instanceWrapper)));
}

} // namespace Microsoft::React::Test
