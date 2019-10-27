// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IWebSocket.h>
#include <cxxreact/JSExecutor.h>
#include <cxxreact/JSModulesUnbundle.h>

// Standard Library
#include <future>
#include <unordered_map>

// OFFICEDEV: Ignore warnings
#pragma warning(push)
#pragma warning(disable : 4290)

using namespace facebook::react;

namespace Microsoft::React {

class ::facebook::react::RAMBundleRegistry;

class WebSocketJSExecutor : public JSExecutor {
 public:
  WebSocketJSExecutor(
      const std::shared_ptr<ExecutorDelegate> &delegate,
      const std::shared_ptr<MessageQueueThread> &messageQueueThread);
  ~WebSocketJSExecutor() override;

#pragma region JSExecutor members

  void loadApplicationScript(
      std::unique_ptr<const JSBigString> script,
#if !defined(OSS_RN)
      uint64_t scriptVersion,
#endif
      std::string sourceURL
#if !defined(OSS_RN)
      ,
      std::string &&bytecodeFileName
#endif
      ) override;
  void setBundleRegistry(std::unique_ptr<RAMBundleRegistry> bundleRegistry) override;
  virtual void registerBundle(uint32_t bundleId, const std::string &bundlePath) override;
  void callFunction(const std::string &moduleId, const std::string &methodId, const folly::dynamic &arguments) override;
  void invokeCallback(const double callbackId, const folly::dynamic &arguments) override;
  void setGlobalVariable(std::string propName, std::unique_ptr<const JSBigString> jsonValue) override;
  void *getJavaScriptContext() override;
  std::string getDescription() override;
#ifdef WITH_JSC_MEMORY_PRESSURE
  void handleMemoryPressure(int pressureLevel) override;
#endif
  void destroy() override;

#pragma endregion // JSExecutor members

  std::future<bool> ConnectAsync(
      const std::string &webSocketServerUrl,
      const std::function<void(std::string)> &errorCallback);

 private:
  void PrepareJavaScriptRuntime();
  std::string Call(const std::string &methodName, folly::dynamic &arguments);
  std::future<std::string> SendMessageAsync(int requestId, std::string &&message);
  void OnMessageReceived(const std::string &msg);
  void flush();

  static const std::string s_description;

  std::shared_ptr<ExecutorDelegate> m_delegate;
  std::shared_ptr<MessageQueueThread> m_messageQueueThread;
  std::unique_ptr<IWebSocket> m_webSocket;
  folly::dynamic m_injectedObjects = folly::dynamic::object;
  std::function<void(std::string)> m_errorCallback;

  std::mutex m_lockPromises;
  std::unordered_map<int, std::promise<std::string>> m_promises;

  enum class State {
    Disconnected,
    Connected, // Websocket is connected, still need to prepare the runtime.
    Running, // Runtime is running.
    Disposed, // Executor has been shutdown.
    Error
  };

  void SetState(State state) noexcept;
  bool IsConnected() const noexcept;
  bool IsDisposed() const noexcept;
  bool IsRunning() const noexcept;
  bool IsInError() const noexcept;

  State m_state = State::Disconnected;

  std::atomic<LONG> m_requestId{0};
};

} // namespace Microsoft::React

#pragma warning(pop)
