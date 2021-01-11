// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <Modules/WebSocketModule.h>
#include "InstanceMocks.h"
#include "WebSocketMocks.h"

using namespace facebook::react;
using namespace facebook::xplat::module;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using folly::dynamic;
using std::function;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

namespace Microsoft::React::Test {

TEST_CLASS (WebSocketModuleTest) {
  const char *MethodName[static_cast<size_t>(WebSocketModule::MethodId::SIZE)]{
      "connect", "close", "send", "sendBinary", "ping"};

  TEST_METHOD(CreateModule) {
    auto module = make_unique<WebSocketModule>();

    Assert::IsFalse(module == nullptr);
    Assert::AreEqual(string("WebSocketModule"), module->getName());

    auto methods = module->getMethods();
    for (size_t i = 0; i < static_cast<size_t>(WebSocketModule::MethodId::SIZE); i++) {
      Assert::AreEqual(string(MethodName[i]), string(methods[i].name));
    }

    Assert::AreEqual(static_cast<size_t>(0), module->getConstants().size());
  }

  TEST_METHOD(ConnectSendsEvent) {
    string eventName;
    string moduleName;
    string methodName;
    auto jsef = make_shared<MockJSExecutorFactory>();
    jsef->CreateJSExecutorMock = [&eventName, &moduleName, &methodName](
                                     shared_ptr<ExecutorDelegate>, shared_ptr<MessageQueueThread>) {
      auto jse = make_unique<MockJSExecutor>();
      jse->CallFunctionMock = [&eventName, &moduleName, &methodName](
                                  const string &module, const string &method, const dynamic &args) {
        moduleName = module;
        methodName = method;
        eventName = args.at(0).asString();
      };

      return std::move(jse);
    };

    auto instance = CreateMockInstance(jsef);
    auto module = make_unique<WebSocketModule>();
    module->setInstance(instance);
    module->SetResourceFactory([](const string &) {
      auto rc = make_shared<MockWebSocketResource>();
      rc->Mocks.Connect = [rc](const IWebSocketResource::Protocols &, const IWebSocketResource::Options &) {
        rc->OnConnect();
      };

      return rc;
    });

    // Execute module method
    auto connect = module->getMethods().at(WebSocketModule::MethodId::Connect);
    connect.func(
        dynamic::array("ws://localhost:0", dynamic(), dynamic(), /*id*/ 0),
        [](vector<dynamic>) {},
        [](vector<dynamic>) {});

    Assert::AreEqual({"RCTDeviceEventEmitter"}, moduleName);
    Assert::AreEqual({"emit"}, methodName);
    Assert::AreEqual({"websocketOpen"}, eventName);
  }
};

} // namespace Microsoft::React::Test
