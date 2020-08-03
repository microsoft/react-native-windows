// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cxxreact/Instance.h>
#include <cxxreact/JSBigString.h>
#include <cxxreact/JSExecutor.h>
#include <cxxreact/MessageQueueThread.h>
#include <cxxreact/RAMBundleRegistry.h>

namespace Microsoft::React::Test {
std::shared_ptr<facebook::react::Instance> CreateMockInstance(std::shared_ptr<facebook::react::JSExecutorFactory> jsef);

class MockMessageQueueThread : public facebook::react::MessageQueueThread {
 public:
#pragma region MessageQueueThread overrides

  void runOnQueue(std::function<void()> &&) override;

  void runOnQueueSync(std::function<void()> &&) override;

  void quitSynchronous() override;

#pragma endregion // MessageQueueThread overrides
};

class MockInstanceCallback : public facebook::react::InstanceCallback {
 public:
#pragma region InstanceCallback overrides

  void onBatchComplete() override;
  void incrementPendingJSCalls() override;
  void decrementPendingJSCalls() override;

#pragma endregion // InstanceCallback overrides
};

class MockJSExecutor : public facebook::react::JSExecutor {
 public:
  std::function<void(const std::string &, const std::string &, const folly::dynamic &)> CallFunctionMock;

#pragma region JSExecutor overrides

  void initializeRuntime() override;

  void loadBundle(std::unique_ptr<const facebook::react::JSBigString> script, std::string sourceURL) override;

  void setBundleRegistry(std::unique_ptr<facebook::react::RAMBundleRegistry> bundleRegistry) override;

  void registerBundle(uint32_t bundleId, const std::string &bundlePath) override;

  void callFunction(const std::string &moduleId, const std::string &methodId, const folly::dynamic &arguments) override;

  void invokeCallback(const double callbackId, const folly::dynamic &arguments) override;

  void setGlobalVariable(std::string propName, std::unique_ptr<const facebook::react::JSBigString> jsonValue) override;

  void *getJavaScriptContext() override;

  bool isInspectable() override;

  std::string getDescription() override;

  void handleMemoryPressure(__unused int pressureLevel) override;

  void destroy() override;

  void flush() override;

#pragma endregion // JSExecutor overrides
};

class MockJSExecutorFactory : public facebook::react::JSExecutorFactory {
 public:
  std::function<std::unique_ptr<facebook::react::JSExecutor>(
      std::shared_ptr<facebook::react::ExecutorDelegate>,
      std::shared_ptr<facebook::react::MessageQueueThread>)>
      CreateJSExecutorMock;

#pragma region JSExecutorFactory overrides

  std::unique_ptr<facebook::react::JSExecutor> createJSExecutor(
      std::shared_ptr<facebook::react::ExecutorDelegate> delegate,
      std::shared_ptr<facebook::react::MessageQueueThread> jsQueue) override;

#pragma endregion // JSExecutorFactory overrides
};

} // namespace Microsoft::React::Test
