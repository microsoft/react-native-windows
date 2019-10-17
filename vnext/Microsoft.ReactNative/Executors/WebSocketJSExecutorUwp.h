// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#pragma warning(push)
#pragma warning(disable : 4146)
#include <DevServerHelper.h>
#include <cxxreact/JSExecutor.h>
#include <cxxreact/JSModulesUnbundle.h>
#include <cxxreact/MessageQueueThread.h>
#pragma warning(pop)

#include <WebSocketJSExecutorFactory.h>

#include <memory>
#include <unordered_map>

#include <winrt/Windows.Networking.Sockets.h>
#include <future>

namespace facebook {
namespace react {
class RAMBundleRegistry;
}
} // namespace facebook

namespace react {
namespace uwp {

class MessageQueueThread;

class WebSocketJSExecutor : public facebook::react::JSExecutor {
 public:
  WebSocketJSExecutor(
      std::shared_ptr<facebook::react::ExecutorDelegate> delegate,
      std::shared_ptr<facebook::react::MessageQueueThread> messageQueueThread);
  ~WebSocketJSExecutor() override;

  virtual void loadApplicationScript(
      std::unique_ptr<const facebook::react::JSBigString> script,
#if !defined(OSS_RN)
      uint64_t scriptVersion,
#endif
      std::string sourceURL
#if !defined(OSS_RN)
      ,
      std::string &&bytecodeFileName
#endif
      ) override;
  virtual void setBundleRegistry(
      std::unique_ptr<facebook::react::RAMBundleRegistry> bundleRegistry)
      override;
  virtual void registerBundle(uint32_t bundleId, const std::string &bundlePath)
      override;
  virtual void callFunction(
      const std::string &moduleId,
      const std::string &methodId,
      const folly::dynamic &arguments) override;
  virtual void invokeCallback(
      const double callbackId,
      const folly::dynamic &arguments) override;
  virtual void setGlobalVariable(
      std::string propName,
      std::unique_ptr<const facebook::react::JSBigString> jsonValue) override;
  virtual void *getJavaScriptContext() override;
  virtual std::string getDescription() override;
#ifdef WITH_JSC_MEMORY_PRESSURE
  virtual void handleMemoryPressure(int pressureLevel) override;
#endif
  virtual void destroy() override;

  winrt::Windows::Foundation::IAsyncAction ConnectAsync(
      const std::string &webSocketServerUrl,
      const std::function<void(std::string)> &errorCallback,
      const std::function<void()> &debuggerAttach,
      const std::function<void()> &waitingForDebuggerCallback);

 private:
  enum class State {
    Disconnected,
    Connected, // Websocket is connected, still need to prepare the runtime.
    Waiting, // Waiting for debugger to connect
    Running, // Runtime is running.
    Disposed, // Executor has been shutdown.
    Error
  };

 private:
  bool PrepareJavaScriptRuntime(int milliseconds);
  void PollPrepareJavaScriptRuntime();
  std::string Call(const std::string &methodName, folly::dynamic &arguments);
  std::future<std::string> SendMessageAsync(
      int requestId,
      const std::string &message);
  void OnMessageReceived(const std::string &msg);
  void flush();

  void SetState(State state) noexcept {
    m_state = state;
  }
  bool IsConnected() const noexcept {
    return m_state == State::Connected;
  }
  bool IsRunning() const noexcept {
    return m_state == State::Running;
  }
  bool IsWaiting() const noexcept {
    return m_state == State::Waiting;
  }
  bool IsInError() const noexcept {
    return m_state == State::Error;
  }
  bool IsDisposed() const noexcept {
    return m_state == State::Disposed;
  }
  void OnHitError(std::string message);
  void OnWaitingForDebugger();
  void OnDebuggerAttach();

  const int ConnectTimeoutMilliseconds = 5000;
  const int ConnectRetryCount = 3;

  std::shared_ptr<facebook::react::ExecutorDelegate> m_delegate;
  std::shared_ptr<facebook::react::MessageQueueThread> m_messageQueueThread;

  // WebSocket
  winrt::Windows::Networking::Sockets::MessageWebSocket m_socket;
  winrt::Windows::Storage::Streams::DataWriter m_socketDataWriter;
  winrt::Windows::Networking::Sockets::MessageWebSocket::MessageReceived_revoker
      m_msgReceived;
  winrt::Windows::Networking::Sockets::MessageWebSocket::Closed_revoker
      m_closed;

  folly::dynamic m_injectedObjects = folly::dynamic::object;

  std::mutex m_lockPromises;
  std::unordered_map<int, std::promise<std::string>> m_promises;

  State m_state = State::Disconnected;
  std::function<void(std::string)> m_errorCallback;
  std::function<void()> m_debuggerAttachCallback;
  std::function<void()> m_waitingForDebuggerCallback;

  std::atomic<LONG> m_requestId{0};
};

} // namespace uwp
} // namespace react
