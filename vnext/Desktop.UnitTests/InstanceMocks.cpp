#include "InstanceMocks.h"

#include <cxxreact/ModuleRegistry.h>

using namespace facebook::react;

using folly::dynamic;
using std::function;
using std::make_shared;
using std::make_unique;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

namespace Microsoft::React::Test {
#pragma region MockMessageQueueThread
void MockMessageQueueThread::runOnQueue(function<void()> &&work) /*override*/
{
  work();
}

void MockMessageQueueThread::runOnQueueSync(function<void()> &&work) /*override*/
{
  work();
}

void MockMessageQueueThread::quitSynchronous() /*override*/ {}
#pragma endregion // MockMessageQueueThread

#pragma region MockInstanceCallback

void MockInstanceCallback::onBatchComplete() /*override*/ {}
void MockInstanceCallback::incrementPendingJSCalls() /*override*/ {}
void MockInstanceCallback::decrementPendingJSCalls() /*override*/ {}

#pragma endregion // MockInstanceCallback

#pragma region MockJSExecutor

void MockJSExecutor::initializeRuntime() {}
void MockJSExecutor::loadBundle(unique_ptr<const JSBigString> script, string sourceURL) {}
void MockJSExecutor::setBundleRegistry(unique_ptr<RAMBundleRegistry> bundleRegistry) {}
void MockJSExecutor::registerBundle(uint32_t bundleId, const string &bundlePath) {}

void MockJSExecutor::callFunction(const string &moduleId, const string &methodId, const dynamic &arguments) {
  if (CallFunctionMock) {
    CallFunctionMock(moduleId, methodId, arguments);
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

#pragma endregion // MockJSExecutor

#pragma region MockJSExecutorFactory

unique_ptr<JSExecutor> MockJSExecutorFactory::createJSExecutor(
    shared_ptr<ExecutorDelegate> delegate,
    shared_ptr<MessageQueueThread> jsQueue) /*override*/
{
  if (CreateJSExecutorMock) {
    return CreateJSExecutorMock(delegate, jsQueue);
  }

  return make_unique<MockJSExecutor>();
}

#pragma endregion // MockJSExecutorFactory

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

} // namespace Microsoft::React::Test
