// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <TestRunner.h>

#include <CreateModules.h>
#include <IUIManager.h>
#include <Modules/NetworkingModule.h>
#include <Modules/WebSocketModule.h>
#include <Threading/MessageQueueThreadFactory.h>
#include <cxxreact/Instance.h>
#include "ChakraRuntimeHolder.h"
#include "DesktopTestInstance.h"
#include "Modules/TestDevSettingsModule.h"
#include "Modules/TestImageLoaderModule.h"
#include "TestInstance.h"
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

struct TestInstanceCallback : public facebook::react::InstanceCallback {
  TestInstanceCallback() {}
  virtual ~TestInstanceCallback() = default;
  void onBatchComplete() override {}
  void incrementPendingJSCalls() override {}
  void decrementPendingJSCalls() override {}
};

shared_ptr<ITestInstance> TestRunner::GetInstance(
    string &&jsBundleFile,
    vector<tuple<string, CxxModule::Provider>> &&cxxModules,
    shared_ptr<DevSettings> devSettings) noexcept {
  auto nativeQueue = react::uwp::MakeJSQueueThread();
  auto jsQueue = react::uwp::MakeJSQueueThread();

  devSettings->jsiRuntimeHolder = std::make_shared<ChakraRuntimeHolder>(devSettings, jsQueue, nullptr, nullptr);

  vector<tuple<string, CxxModule::Provider, shared_ptr<MessageQueueThread>>> extraModules{
      {"AsyncLocalStorage",
       []() -> unique_ptr<CxxModule> {
         return /*CreateAsyncStorageModule(L"ReactNativeAsyncStorage")*/ nullptr; // #6882
       },
       nativeQueue},

      {"WebSocketModule", []() -> unique_ptr<CxxModule> { return std::make_unique<WebSocketModule>(); }, nativeQueue},

      {"Networking",
       []() -> unique_ptr<CxxModule> { return std::make_unique<Microsoft::React::NetworkingModule>(); },
       nativeQueue},

      {"Timing", [nativeQueue]() -> unique_ptr<CxxModule> { return CreateTimingModule(nativeQueue); }, nativeQueue},

      // Apparently mandatory for /IntegrationTests
      {TestAppStateModule::name,
       []() -> unique_ptr<CxxModule> { return std::make_unique<TestAppStateModule>(); },
       nativeQueue},

      // Apparently mandatory for /IntegrationTests
      {"UIManager", []() -> unique_ptr<CxxModule> { return std::make_unique<TestUIManager>(); }, nativeQueue},

      // Apparently mandatory for /IntegrationTests
      {TestDeviceInfoModule::name,
       []() -> unique_ptr<CxxModule> { return std::make_unique<TestDeviceInfoModule>(); },
       nativeQueue},

      {TestDevSettingsModule::name,
       []() -> unique_ptr<CxxModule> { return std::make_unique<TestDevSettingsModule>(); },
       nativeQueue},

      {TestImageLoaderModule::name,
       []() -> unique_ptr<CxxModule> { return std::make_unique<TestImageLoaderModule>(); },
       nativeQueue}};

  // <0> string
  // <1> CxxModule::Provider
  for (auto &t : cxxModules) {
    extraModules.emplace_back(std::get<0>(t), std::get<1>(t), nativeQueue);
  }

  // Update settings.
  devSettings->platformName = "windows";

  auto instanceWrapper = CreateReactInstance(
      std::make_shared<facebook::react::Instance>(),
      "",
      std::move(extraModules),
      nullptr,
      std::make_unique<TestInstanceCallback>(),
      std::move(jsQueue),
      std::move(nativeQueue),
      std::move(devSettings));
  instanceWrapper->loadBundle(std::move(jsBundleFile));

  return shared_ptr<ITestInstance>(new DesktopTestInstance(std::move(instanceWrapper)));
}

} // namespace Microsoft::React::Test
