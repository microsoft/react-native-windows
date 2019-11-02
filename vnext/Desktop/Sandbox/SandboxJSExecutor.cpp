// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "SandboxBridge.h"
#include "SandboxJSExecutor.h"

#include <cxxreact/JSBigString.h>
#include <cxxreact/ModuleRegistry.h>
#include <cxxreact/RAMBundleRegistry.h>

#include <agents.h>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <chrono>

using namespace std;
using namespace Concurrency;

// OFFICEDEV: Ignore warnings
#pragma warning(push)
#pragma warning(disable : 4100 4101 4244 4290 4456)

namespace facebook {
namespace react {

#if !defined(OSS_RN)

std::unique_ptr<ExecutorDelegate> SandboxDelegateFactory::createExecutorDelegate(
    std::shared_ptr<ModuleRegistry> registry,
    std::shared_ptr<InstanceCallback> callback) {
  return std::unique_ptr<ExecutorDelegate>(new SandboxJsToNativeBridge(registry, callback, m_sendNativeModuleCall));
}

std::unique_ptr<ExecutorDelegate> SandboxExecutorDelegateFactory::createExecutorDelegate(
    std::shared_ptr<ModuleRegistry> registry,
    std::shared_ptr<InstanceCallback> callback) {
  return std::unique_ptr<ExecutorDelegate>(new SandboxHostNativeBridge(registry, callback));
}

SandboxJSExecutorFactory::SandboxJSExecutorFactory(JSECreator &&jsExecutorFactory)
    : m_jseCreater(std::move(jsExecutorFactory)) {}

std::unique_ptr<JSExecutor> SandboxJSExecutorFactory::createJSExecutor(
    std::shared_ptr<ExecutorDelegate> delegate,
    std::shared_ptr<MessageQueueThread> jsQueue) {
  if (m_jseCreater) {
    return m_jseCreater(delegate, jsQueue);
  } else {
    return std::unique_ptr<JSExecutor>(new SandboxJSExecutor(delegate, jsQueue));
  }
}

template <typename Func>
auto create_delayed_task(
    std::chrono::milliseconds delay,
    Func func,
    concurrency::cancellation_token token = concurrency::cancellation_token::none()) -> decltype(create_task(func)) {
  concurrency::task_completion_event<void> tce;

  auto pTimer = new concurrency::timer<int>(static_cast<int>(delay.count()), 0, NULL, false);
  auto pCallback = new concurrency::call<int>([tce](int) { tce.set(); });

  pTimer->link_target(pCallback);
  pTimer->start();
  token.register_callback([tce]() { tce.set(); });

  return create_task(tce)
      .then([pCallback, pTimer]() {
        delete pCallback;
        delete pTimer;
      })
      .then(func, token);
}

SandboxJSExecutor::SandboxJSExecutor(
    std::shared_ptr<ExecutorDelegate> delegate,
    std::shared_ptr<MessageQueueThread> messageQueueThread)
    : m_delegate(delegate), m_messageQueueThread(messageQueueThread) {}

SandboxJSExecutor::~SandboxJSExecutor() {}

void SandboxJSExecutor::loadApplicationScript(
    std::unique_ptr<const JSBigString> script,
    std::string sourceURL) {
  auto requestId = GetNextRequestId();
  task_completion_event<void> callback;
  m_callbacks.emplace(requestId, callback);

  folly::dynamic request =
      folly::dynamic::object("script", script->c_str())("inject", m_injectedObjects)("url", sourceURL);

  try {
    SendMessageAsync(requestId, "executeApplicationScript", request)
        .then([callback](bool success) {
          if (success) {
            // Return task to wait for reply.
            return task<void>(callback);
          } else {
            throw exception("Failed to send");
          }
        })
        .get();
  }
  // TODO: handle exceptions in a better way
  catch (exception &e) {
    m_errorCallback(e.what());
    SetState(State::Error);
  }
}

void SandboxJSExecutor::registerBundle(uint32_t /*bundleId*/, const std::string & /*bundlePath*/) {
  // NYI
  std::terminate();
}

void SandboxJSExecutor::setBundleRegistry(std::unique_ptr<RAMBundleRegistry> bundleRegistry) {}

void SandboxJSExecutor::callFunction(
    const std::string &moduleId,
    const std::string &methodId,
    const folly::dynamic &arguments) {
  if (IsInError()) {
    return;
  }
  folly::dynamic jarray = folly::dynamic::array(moduleId, methodId, arguments);
  Call("callFunction", jarray);
}

void SandboxJSExecutor::invokeCallback(const double callbackId, const folly::dynamic &arguments) {
  if (IsInError()) {
    return;
  }
  folly::dynamic jarray = folly::dynamic::array(callbackId, arguments);
  Call("invokeCallback", jarray);
}

void SandboxJSExecutor::setGlobalVariable(std::string propName, std::unique_ptr<const JSBigString> jsonValue) {
  m_injectedObjects[propName] = std::string(jsonValue->c_str());
}

void *SandboxJSExecutor::getJavaScriptContext() {
  return nullptr;
}

std::string SandboxJSExecutor::getDescription() {
  return "SandboxJSExecutor";
}

#ifdef WITH_JSC_MEMORY_PRESSURE
void SandboxJSExecutor::handleMemoryPressure(int pressureLevel) {}
#endif

void SandboxJSExecutor::destroy() {
  SetState(State::Disposed);
  if (m_sandboxEndpoint) {
    m_sandboxEndpoint->Shutdown();
  }
}

// NOTE: This function synchronously waiting until get reply back from the
// sandbox.
// TODO: Indefinite waiting can cause SDX to become unresponsive. We need to
// implement timeout mechanism.
void SandboxJSExecutor::Call(const std::string &methodName, folly::dynamic &arguments) {
  auto requestId = GetNextRequestId();
  task_completion_event<void> callback;
  m_callbacks.emplace(requestId, callback);

  try {
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    SendMessageAsync(requestId, methodName, arguments)
        .then([callback](bool sent) {
          if (sent) {
            // Return task to wait for reply.
            return task<void>(callback);
          } else {
            throw exception("Failed to send");
          }
        })
        .get(); // wait until get reply

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    char buffer[256];
    sprintf_s(buffer, "(%03I64d) CallFunction, elapsed time = %d us\n", requestId, (int)duration);
    OutputDebugStringA(buffer);
  }
  // TODO: handle exceptions in a better way
  catch (exception &e) {
    m_errorCallback(e.what());
    SetState(State::Error);
  }
}

void SandboxJSExecutor::SetState(State state) noexcept {
  m_state = state;
}

bool SandboxJSExecutor::IsListening() const noexcept {
  return m_state == State::Listening;
}

bool SandboxJSExecutor::IsConnected() const noexcept {
  return m_state == State::Connected;
}

bool SandboxJSExecutor::IsDisposed() const noexcept {
  return m_state == State::Disposed;
}

bool SandboxJSExecutor::IsInError() const noexcept {
  return m_state == State::Error;
}

inline int64_t SandboxJSExecutor::GetNextRequestId() {
  return ++m_requestId;
}

bool SandboxJSExecutor::IsRunning() const noexcept {
  return m_state == State::Running;
}

task<bool> SandboxJSExecutor::ConnectAsync(
    std::shared_ptr<SandboxEndpoint> endpoint,
    const std::function<void(std::string)> &errorCallback) {
  m_errorCallback = std::move(errorCallback);
  auto t = task_from_result();

  return t.then([=]() -> bool {
    int retryCount = ConnectRetryCount;
    while (true) {
      try {
        cancellation_token_source timer_cts;
        auto timeoutT = create_delayed_task(
            std::chrono::milliseconds(ConnectTimeoutMilliseconds),
            [=]() -> string { throw std::runtime_error("timeout"); },
            timer_cts.get_token());

        if (!IsConnected()) {
          try {
            m_sandboxEndpoint = nullptr;
          } catch (std::exception & /*e*/) {
            // Don't care what happens with the old client at this point
          }

          // TODO: Pass as param
          m_sandboxEndpoint = endpoint;

          m_sandboxEndpoint->RegisterReplyHandler([this](int64_t replyId) { OnReplyMessage(replyId); });

          m_sandboxEndpoint->RegisterNativeModuleCallHandler(
              [this](folly::dynamic &&calls) { OnNativeModuleCallMessage(std::move(calls)); });

          if (m_sandboxEndpoint->Start(EndpointType::Host)) {
            SetState(State::Connected);
            timer_cts.cancel();
          }
        } else {
          PrepareJavaScriptRuntimeAsync().then([=](bool success) {
            if (success) {
              SetState(State::Running);
              timer_cts.cancel();
            } else {
              SetState(State::Error);
            }
          });
        }

        auto status = timeoutT.wait();

        if (status != canceled) {
          throw new std::exception("Timeout");
        }

        if (IsRunning()) {
          return true;
        }
      } catch (std::exception & /*e*/) {
        retryCount--;
        if (retryCount == 0) {
          m_errorCallback(IsConnected() ? "Timeout: preparing JS runtime" : "Timeout: Failed to connect to dev server");
          SetState(State::Error);
          return false;
        }
      }
    }
  });
}

task<bool> SandboxJSExecutor::PrepareJavaScriptRuntimeAsync() {
  task_completion_event<void> callback;
  auto requestId = GetNextRequestId();
  m_callbacks.emplace(requestId, callback);
  folly::dynamic argument;
  return SendMessageAsync(requestId, "prepareJSRuntime", argument);
}

task<bool>
SandboxJSExecutor::SendMessageAsync(int64_t requestId, const std::string &methodName, folly::dynamic &arguments) {
  if (!IsDisposed()) {
    task_completion_event<bool> tce;
    m_sandboxEndpoint->SendRequest(requestId, methodName, arguments, [tce](bool sent) { tce.set(sent); });
    return create_task(tce);
  } else {
    CompleteRequest(requestId);
    return task_from_result(false);
  }
}

void SandboxJSExecutor::CompleteRequest(int64_t requestId) {
  auto it = m_callbacks.find(requestId);
  if (it != m_callbacks.end()) {
    it->second.set();
    m_callbacks.erase(it);
  }
}

void SandboxJSExecutor::OnReplyMessage(int64_t replyId) {
  // Find callback by replyId and set signal.
  CompleteRequest(replyId);
}

void SandboxJSExecutor::OnNativeModuleCallMessage(folly::dynamic &&calls) {
  m_delegate->callNativeModules(*this, std::move(calls), false);
}

#endif // OSS_RN
} // namespace react
} // namespace facebook
#pragma warning(pop)
