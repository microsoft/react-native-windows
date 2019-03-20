// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <WebSocketModule.h>

using namespace facebook::react;
using namespace folly;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::make_unique;
using std::unique_ptr;
using std::vector;

TEST_CLASS(WebSocketModuleIntegrationTest)
{
  TEST_METHOD(WebSocketModule_Ping)
  {
    auto module = make_unique<WebSocketModule>();

    auto connect = module->getMethods().at(WebSocketModule::MethodId::Connect);
    connect.func(dynamic::array("ws://localhost:5555/", dynamic(), dynamic(), /*id*/ 0), [](vector<dynamic>){}, [](vector<dynamic>){});

    auto ping = module->getMethods().at(WebSocketModule::MethodId::Ping);
    ping.func(dynamic::array(0), [](vector<dynamic>){}, [](vector<dynamic>){});

    auto close = module->getMethods().at(WebSocketModule::MethodId::Close);
    close.func(dynamic::array(0, "closing", /*id*/ 0), [](vector<dynamic>) {}, [](vector<dynamic>) {});
  }

  TEST_METHOD(WebSocketModule_SendMultiple)
  {
    auto module = make_unique<WebSocketModule>();

    auto connect = module->getMethods().at(WebSocketModule::MethodId::Connect);
    connect.func(dynamic::array("ws://localhost:5555/", dynamic(), dynamic(), /*id*/ 0), [](vector<dynamic>) {}, [](vector<dynamic>) {});
    connect.func(dynamic::array("ws://localhost:5555/", dynamic(), dynamic(), /*id*/ 1), [](vector<dynamic>){}, [](vector<dynamic>){});

    auto send = module->getMethods().at(WebSocketModule::MethodId::Send);
    send.func(dynamic::array("request1", 0), [](vector<dynamic>) {}, [](vector<dynamic>) {});
    send.func(dynamic::array("request2", 0), [](vector<dynamic>) {}, [](vector<dynamic>) {});
    send.func(dynamic::array("request3", 1), [](vector<dynamic>) {}, [](vector<dynamic>) {});
    send.func(dynamic::array("request4", 1), [](vector<dynamic>) {}, [](vector<dynamic>) {});

    auto close = module->getMethods().at(WebSocketModule::MethodId::Close);
    close.func(dynamic::array(0, "closing", /*id*/ 0), [](vector<dynamic>) {}, [](vector<dynamic>) {});
    close.func(dynamic::array(0, "closing", /*id*/ 1), [](vector<dynamic>) {}, [](vector<dynamic>) {});
  }
};
