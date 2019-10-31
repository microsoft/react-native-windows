// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>

#include <Executors/WebSocketJSExecutor.h>
#include <cxxreact/JSBigString.h>
#include "MockExecutorDelegate.h"
#include "TestMessageQueueThread.h"

using namespace facebook::react;
using namespace Microsoft::React;
using namespace Microsoft::React::Test;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using std::make_shared;
using std::make_unique;
using std::move;
using std::string;
using std::unique_ptr;

TEST_CLASS(WebSocketJSExecutorIntegrationTest){
    BEGIN_TEST_METHOD_ATTRIBUTE(ConnectAsyncSucceeds) END_TEST_METHOD_ATTRIBUTE()
        TEST_METHOD(ConnectAsyncSucceeds){auto jsThread = make_shared<TestMessageQueueThread>();
auto delegate = make_shared<MockDelegate>();
auto jse = make_unique<WebSocketJSExecutor>(delegate, jsThread);

string errorMessage;
auto errorCallback = [&errorMessage](string message) { errorMessage = message; };
bool connected = jse->ConnectAsync("ws://localhost:8081/debugger-proxy?role=client", move(errorCallback)).get();
jsThread->quitSynchronous();

Assert::AreEqual({}, errorMessage);
Assert::IsTrue(connected);
}

BEGIN_TEST_METHOD_ATTRIBUTE(ConnectAsyncFails)
END_TEST_METHOD_ATTRIBUTE()
TEST_METHOD(ConnectAsyncFails) {
  auto jsThread = make_shared<TestMessageQueueThread>();
  auto delegate = make_shared<MockDelegate>();
  auto jse = make_unique<WebSocketJSExecutor>(delegate, jsThread);

  string errorMessage;
  auto errorCallback = [&errorMessage](string message) { errorMessage = message; };
  bool connected = jse->ConnectAsync("ws://localhost:0/", move(errorCallback)).get();
  jsThread->quitSynchronous();

  Assert::AreNotEqual({}, errorMessage);
  Assert::IsFalse(connected);
}

BEGIN_TEST_METHOD_ATTRIBUTE(LoadApplicationScriptSucceeds)
END_TEST_METHOD_ATTRIBUTE()
TEST_METHOD(LoadApplicationScriptSucceeds) {
  auto jsQueue = make_shared<TestMessageQueueThread>();
  auto delegate = make_shared<MockDelegate>();
  auto jse = make_unique<WebSocketJSExecutor>(delegate, jsQueue);
  auto bridgeConfig = std::unique_ptr<JSBigString>(new JSBigStdString("{}"));
  jse->setGlobalVariable("__fbBatchedBridgeConfig", move(bridgeConfig));

  string errorMessage;
  auto errorCallback = [&errorMessage](string message) { errorMessage = message; };
  bool connected = jse->ConnectAsync("ws://localhost:8081/debugger-proxy?role=client", move(errorCallback)).get();
  // Point to an existing script accessible via the repository's packaging
  // service.
  auto bigString = unique_ptr<JSBigString>(
      new JSBigStdString("http://localhost:8081/IntegrationTests/IntegrationTestsAppWin.bundle?platform=ios&dev=true"));
  jse->loadApplicationScript(
      std::move(bigString),
#if !defined(OSS_RN)
      0,
#endif
      "");

  jsQueue->quitSynchronous();

  Assert::IsTrue(connected);
  Assert::AreEqual({}, errorMessage);
}

BEGIN_TEST_METHOD_ATTRIBUTE(LoadApplicationScriptHandles404)
TEST_IGNORE()
END_TEST_METHOD_ATTRIBUTE()
TEST_METHOD(LoadApplicationScriptHandles404) {
  auto jsThread = make_shared<TestMessageQueueThread>();
  auto delegate = make_shared<MockDelegate>();
  auto jse = make_unique<WebSocketJSExecutor>(delegate, jsThread);

  string errorMessage;
  auto errorCallback = [&errorMessage](string message) { errorMessage = message; };
  bool connected = jse->ConnectAsync("ws://localhost:8081/debugger-proxy?role=client", move(errorCallback)).get();
  // Point to a non-existing path.
  auto bigString = unique_ptr<JSBigString>(new JSBigStdString("http://localhost:8081/showme404"));
  jse->loadApplicationScript(
      std::move(bigString),
#if !defined(OSS_RN)
      0,
#endif
      "");

  jsThread->quitSynchronous();

  Assert::IsTrue(connected);
  Assert::AreNotEqual({}, errorMessage);
}

BEGIN_TEST_METHOD_ATTRIBUTE(LoadApplicationScriptHandlesNonExistingBundle)
TEST_IGNORE()
END_TEST_METHOD_ATTRIBUTE()
TEST_METHOD(LoadApplicationScriptHandlesNonExistingBundle) {
  auto jsThread = make_shared<TestMessageQueueThread>();
  auto delegate = make_shared<MockDelegate>();
  auto jse = make_unique<WebSocketJSExecutor>(delegate, jsThread);

  string errorMessage;
  auto errorCallback = [&errorMessage](string message) { errorMessage = message; };
  bool connected = jse->ConnectAsync("ws://localhost:8081/debugger-proxy?role=client", move(errorCallback)).get();
  // Point to a non-existing bundle.
  auto bigString = unique_ptr<JSBigString>(new JSBigStdString("http://localhost:8081/nonexisting.bundle"));
  jse->loadApplicationScript(
      std::move(bigString),
#if !defined(OSS_RN)
      0,
#endif
      "");

  jsThread->quitSynchronous();

  Assert::IsTrue(connected);
  Assert::AreNotEqual({}, errorMessage);
}
}
;
