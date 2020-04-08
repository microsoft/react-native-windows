// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <CppUnitTest.h>
#include <Modules/WebSocketModule.h>

// TODO: Move out
#include <cxxreact/Instance.h>
#include <cxxreact/JSBigString.h>
#include <cxxreact/JSExecutor.h>
#include <cxxreact/MessageQueueThread.h>
#include <cxxreact/ModuleRegistry.h>
#include <cxxreact/RAMBundleRegistry.h>

using namespace facebook::react;
using namespace facebook::xplat::module;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft::React::Test {

class MockMessageQueueThread : public MessageQueueThread {
 public:
#pragma region MessageQueueThread overrides

  void runOnQueue(std::function<void()> &&) override;

  void runOnQueueSync(std::function<void()> &&) override;

  void quitSynchronous() override;

#pragma endregion // MessageQueueThread overrides
};

class MockInstanceCallback : public InstanceCallback {
 public:
#pragma region InstanceCallback overrides

  void onBatchComplete() override;
  void incrementPendingJSCalls() override;
  void decrementPendingJSCalls() override;

#pragma endregion // InstanceCallback overrides
};

class MockJSExecutor : public JSExecutor {
 public:
  std::function<void(const std::string &, const std::string &, const folly::dynamic &)> CallFunctionFunctor;

#pragma region JSExecutor overrides

  void loadApplicationScript(std::unique_ptr<const JSBigString> script, std::string sourceURL) override;

  void setBundleRegistry(std::unique_ptr<RAMBundleRegistry> bundleRegistry) override;

  void registerBundle(uint32_t bundleId, const std::string &bundlePath) override;

  void callFunction(const std::string &moduleId, const std::string &methodId, const folly::dynamic &arguments) override;

  void invokeCallback(const double callbackId, const folly::dynamic &arguments) override;

  void setGlobalVariable(std::string propName, std::unique_ptr<const JSBigString> jsonValue) override;

  void *getJavaScriptContext() override;

  bool isInspectable() override;

  std::string getDescription() override;

  void handleMemoryPressure(__unused int pressureLevel) override;

  void destroy() override;

  void flush() override;

#pragma endregion // JSExecutor overrides
};

class MockJSExecutorFactory : public JSExecutorFactory {
 public:
  std::function<std::unique_ptr<JSExecutor>(std::shared_ptr<ExecutorDelegate>, std::shared_ptr<MessageQueueThread>)>
      CreateJSExecutorFunction;

#pragma region JSExecutorFactory overrides

  std::unique_ptr<JSExecutor> createJSExecutor(
      std::shared_ptr<ExecutorDelegate> delegate,
      std::shared_ptr<MessageQueueThread> jsQueue) override;

#pragma endregion // JSExecutorFactory overrides
};

#pragma region Move

// MockMessageQueueThread
using folly::dynamic;
using std::function;
using std::string;

void MockMessageQueueThread::runOnQueue(function<void()> &&) /*override*/ {}
void MockMessageQueueThread::runOnQueueSync(function<void()> &&work) /*override*/
{
  work();
}

void MockMessageQueueThread::quitSynchronous() /*override*/ {}

// MockInstanceCallback
void MockInstanceCallback::onBatchComplete() /*override*/ {}
void MockInstanceCallback::incrementPendingJSCalls() /*override*/ {}
void MockInstanceCallback::decrementPendingJSCalls() /*override*/ {}

// MockJSExecutor
void MockJSExecutor::loadApplicationScript(unique_ptr<const JSBigString> script, string sourceURL) {}
void MockJSExecutor::setBundleRegistry(unique_ptr<RAMBundleRegistry> bundleRegistry) {}
void MockJSExecutor::registerBundle(uint32_t bundleId, const string &bundlePath) {}

void MockJSExecutor::callFunction(const string &moduleId, const string &methodId, const dynamic &arguments) {
  if (CallFunctionFunctor) {
    CallFunctionFunctor(moduleId, methodId, arguments);
  }
}

void MockJSExecutor::invokeCallback(const double callbackId, const dynamic &arguments) {}
void MockJSExecutor::setGlobalVariable(string propName, unique_ptr<const JSBigString> jsonValue) {}
void *MockJSExecutor::getJavaScriptContext() {
  return nullptr;
}

bool MockJSExecutor::isInspectable() {
  return false;
}
std::string MockJSExecutor::getDescription() {
  return {};
}
void MockJSExecutor::handleMemoryPressure(__unused int pressureLevel) {}
void MockJSExecutor::destroy() {}
void MockJSExecutor::flush() {}

// MockJSExecutorFactory

using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::unique_ptr;
using std::vector;

unique_ptr<JSExecutor> MockJSExecutorFactory::createJSExecutor(
    shared_ptr<ExecutorDelegate> delegate,
    shared_ptr<MessageQueueThread> jsQueue) /*override*/
{
  if (CreateJSExecutorFunction) {
    return CreateJSExecutorFunction(delegate, jsQueue);
  }

  return make_unique<MockJSExecutor>();
}

// MockInstance.cpp ?

shared_ptr<Instance> CreateMockInstance(shared_ptr<JSExecutorFactory> jsef) {
  auto instance = make_shared<Instance>();
  auto callback = make_unique<MockInstanceCallback>();
  auto jsQueue = make_shared<MockMessageQueueThread>();
  auto moduleRegistry = make_shared<ModuleRegistry>(
      vector<unique_ptr<NativeModule>>(), // modules
      nullptr // moduleNotFoundCallback
  );

  instance->initializeBridge(std::move(callback), jsef, jsQueue, moduleRegistry);

  return instance;
}

#pragma endregion // Move

TEST_CLASS (WebSocketModuleTest) {
  enum class MethodId : size_t { Connect = 0, Close = 1, Send = 2, SendBinary = 3, Ping = 4, SIZE = 5 };

  const char *MethodName[static_cast<size_t>(MethodId::SIZE)]{"connect", "close", "send", "sendBinary", "ping"};

  TEST_METHOD(WebSocketModuleTest_CreateModule) {
    auto module = make_unique<WebSocketModule>();

    Assert::IsFalse(module == nullptr);
    Assert::AreEqual(string("WebSocketModule"), module->getName());

    auto methods = module->getMethods();
    for (size_t i = 0; i < static_cast<size_t>(MethodId::SIZE); i++) {
      Assert::AreEqual(string(MethodName[i]), string(methods[i].name));
    }

    Assert::AreEqual(static_cast<size_t>(0), module->getConstants().size());
  }

  TEST_METHOD(WebSocketModuleDummyRemove) {
    auto jsef = make_shared<MockJSExecutorFactory>();
    auto instance = CreateMockInstance(jsef);

    jsef->CreateJSExecutorFunction = [](shared_ptr<ExecutorDelegate>, shared_ptr<MessageQueueThread>) {
      auto jse = make_unique<MockJSExecutor>();
      jse->CallFunctionFunctor = [](const string &, const string &, const dynamic &) {
        // TODO: Handle callFunction
      };

      return std::move(jse);
    };

    auto module = make_unique<WebSocketModule>();
    // TODO: Mock member WebSocket resource.
  }
};

} // namespace Microsoft::React::Test
