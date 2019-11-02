// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include <DevSupportManager.h>
#include <cxxreact/JSExecutor.h>
#include <cxxreact/JSModulesUnbundle.h>
#include "Sandbox/SandboxEndpoint.h"

// OFFICEDEV: Ignore warnings
#pragma warning(push)
#pragma warning(disable : 4290)

namespace facebook {
namespace react {

class MessageQueueThread;

#if !defined(OSS_RN)

// Used by sandbox process to handle NativeModule calls from JS.
class SandboxDelegateFactory : public ExecutorDelegateFactory {
 public:
  SandboxDelegateFactory() = delete;
  explicit SandboxDelegateFactory(SendNativeModuleCall &&sendNativeModuleCall)
      : m_sendNativeModuleCall(std::move(sendNativeModuleCall)) {}

  virtual std::unique_ptr<ExecutorDelegate> createExecutorDelegate(
      std::shared_ptr<ModuleRegistry>,
      std::shared_ptr<InstanceCallback>) override;

 private:
  SendNativeModuleCall m_sendNativeModuleCall;
};

// Used by SandboxExecutor in host to handle NativeModule calls from sandbox.
class SandboxExecutorDelegateFactory : public ExecutorDelegateFactory {
 public:
  SandboxExecutorDelegateFactory() = default;

  virtual std::unique_ptr<ExecutorDelegate> createExecutorDelegate(
      std::shared_ptr<ModuleRegistry>,
      std::shared_ptr<InstanceCallback>) override;
};

using JSECreator = std::function<std::unique_ptr<facebook::react::JSExecutor>(
    std::shared_ptr<facebook::react::ExecutorDelegate>,
    std::shared_ptr<facebook::react::MessageQueueThread>)>;

class SandboxJSExecutorFactory : public JSExecutorFactory {
 public:
  SandboxJSExecutorFactory() = default;
  SandboxJSExecutorFactory(JSECreator &&jsExecutorFactory);

  virtual std::unique_ptr<JSExecutor> createJSExecutor(
      std::shared_ptr<ExecutorDelegate> delegate,
      std::shared_ptr<MessageQueueThread> jsQueue) override;

 private:
  JSECreator m_jseCreater;
};

class SandboxJSExecutor : public JSExecutor {
 public:
  SandboxJSExecutor(std::shared_ptr<ExecutorDelegate> delegate, std::shared_ptr<MessageQueueThread> messageQueueThread);
  ~SandboxJSExecutor() override;

  virtual void loadApplicationScript(
      std::unique_ptr<const JSBigString> script,
      std::string sourceURL) override;
  virtual void setBundleRegistry(std::unique_ptr<RAMBundleRegistry> bundleRegistry) override;
  virtual void registerBundle(uint32_t bundleId, const std::string &bundlePath) override;
  virtual void callFunction(const std::string &moduleId, const std::string &methodId, const folly::dynamic &arguments)
      override;
  virtual void invokeCallback(const double callbackId, const folly::dynamic &arguments) override;
  virtual void setGlobalVariable(std::string propName, std::unique_ptr<const JSBigString> jsonValue) override;
  virtual void *getJavaScriptContext() override;
  virtual std::string getDescription() override;
#ifdef WITH_JSC_MEMORY_PRESSURE
  virtual void handleMemoryPressure(int pressureLevel) override;
#endif
  virtual void destroy() override;

  Concurrency::task<bool> ConnectAsync(
      std::shared_ptr<SandboxEndpoint> endpoint,
      const std::function<void(std::string)> &errorCallback);

 private:
  Concurrency::task<bool> PrepareJavaScriptRuntimeAsync();
  void Call(const std::string &methodName, folly::dynamic &arguments);
  Concurrency::task<bool> SendMessageAsync(int64_t requestId, const std::string &methodName, folly::dynamic &arguments);
  void CompleteRequest(int64_t requestId);

  void OnReplyMessage(int64_t replyId);
  void OnNativeModuleCallMessage(folly::dynamic &&calls);

  const int ConnectTimeoutMilliseconds = 5000;
  const int ConnectRetryCount = 3;

  std::shared_ptr<ExecutorDelegate> m_delegate;
  std::shared_ptr<MessageQueueThread> m_messageQueueThread;
  std::shared_ptr<SandboxEndpoint> m_sandboxEndpoint;
  folly::dynamic m_injectedObjects = folly::dynamic::object;
  std::function<void(std::string)> m_errorCallback;

  std::map<int64_t, Concurrency::task_completion_event<void>> m_callbacks;

  enum class State {
    Disconnected,
    Listening,
    Connected, // Sandbox is connected, still need to prepare the runtime
    Running, // Runtime is running
    Disposed, // Executor has been shutdown
    Error,
  };
  std::atomic<State> m_state{State::Disconnected};

  void SetState(State state) noexcept;
  bool IsListening() const noexcept;
  bool IsConnected() const noexcept;
  bool IsDisposed() const noexcept;
  bool IsRunning() const noexcept;
  bool IsInError() const noexcept;

  int64_t GetNextRequestId();
  std::atomic<int64_t> m_requestId{1}; // RequestId starting from 1
};

#endif // OSS_RN

} // namespace react
} // namespace facebook
#pragma warning(pop)
