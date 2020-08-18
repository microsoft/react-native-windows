// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <cxxreact/JSBigString.h>
#include <cxxreact/RAMBundleRegistry.h>
#include "WebSocketJSExecutor.h"

#include <folly/dynamic.h>
#include <folly/json.h>

#include "Unicode.h"
#include "Utilities.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>

// Hx/OFFICEDEV: Ignore warnings
#pragma warning(push)
#pragma warning(disable : 4100 4101 4244 4290 4456)

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace react::uwp {

WebSocketJSExecutor::WebSocketJSExecutor(
    std::shared_ptr<facebook::react::ExecutorDelegate> delegate,
    std::shared_ptr<facebook::react::MessageQueueThread> messageQueueThread)
    : m_delegate(delegate),
      m_messageQueueThread(messageQueueThread),
      m_socket(),
      m_socketDataWriter(m_socket.OutputStream()) {
  m_msgReceived = m_socket.MessageReceived(winrt::auto_revoke, [this](auto &&, auto &&args) {
    try {
      std::string response;
      if (args.MessageType() == winrt::Windows::Networking::Sockets::SocketMessageType::Utf8) {
        winrt::Windows::Storage::Streams::DataReader reader = args.GetDataReader();
        reader.UnicodeEncoding(winrt::Windows::Storage::Streams::UnicodeEncoding::Utf8);
        uint32_t len = reader.UnconsumedBufferLength();
        std::vector<uint8_t> data(len);
        reader.ReadBytes(data);

        std::string str(Microsoft::Common::Utilities::CheckedReinterpretCast<char *>(data.data()), data.size());
        OnMessageReceived(str);
      } else {
        OnHitError("Unexpected MessageType from MessageWebSocket.");
      }
    } catch (winrt::hresult_error const &e) {
      auto hr = e.code();
      if (hr == WININET_E_CONNECTION_ABORTED || hr == WININET_E_CONNECTION_RESET) {
        OnHitError("Lost connection to remote JS debugger.");
      } else {
        OnHitError(Microsoft::Common::Unicode::Utf16ToUtf8(e.message().c_str(), e.message().size()));
      }
    } catch (std::exception &e) {
      OnHitError(e.what());
    }
  });

  auto weakThis = weak_from_this();
  m_closed = m_socket.Closed(winrt::auto_revoke, [weakThis](auto &&, auto &&args) {
    if (auto _this = weakThis.lock()) {
      _this->SetState(State::Disconnected);
    }
  });
}

WebSocketJSExecutor::~WebSocketJSExecutor() {
  m_closed.revoke();
  m_msgReceived.revoke();
}

void WebSocketJSExecutor::initializeRuntime() {
  // No init needed before loading a bundle
}

void WebSocketJSExecutor::loadBundle(
    std::unique_ptr<const facebook::react::JSBigString> script,
    std::string sourceURL) {
  int requestId = ++m_requestId;

  if (!IsRunning()) {
    OnHitError("Executor instance not connected to a WebSocket endpoint.");
    return;
  }

  try {
    folly::dynamic request = folly::dynamic::object("id", requestId)("method", "executeApplicationScript")(
        "url", script->c_str())("inject", m_injectedObjects);
    std::string str = folly::toJson(request);
    std::string strReturn = SendMessageAsync(requestId, str).get();
  } catch (const std::exception &e) {
    OnHitError(e.what());
  }
}

void WebSocketJSExecutor::setBundleRegistry(std::unique_ptr<facebook::react::RAMBundleRegistry> bundleRegistry) {}

void WebSocketJSExecutor::registerBundle(uint32_t bundleId, const std::string &bundlePath) {
  // NYI
  std::terminate();
}

void WebSocketJSExecutor::flush() {
  folly::dynamic jarray = folly::dynamic::array();
  auto calls = Call("flushedQueue", jarray);
  if (m_delegate && !IsInError())
    m_delegate->callNativeModules(*this, folly::parseJson(std::move(calls)), true);
}

void WebSocketJSExecutor::callFunction(
    const std::string &moduleId,
    const std::string &methodId,
    const folly::dynamic &arguments) {
  folly::dynamic jarray = folly::dynamic::array(moduleId, methodId, arguments);
  auto calls = Call("callFunctionReturnFlushedQueue", jarray);
  if (m_delegate && !IsInError())
    m_delegate->callNativeModules(*this, folly::parseJson(std::move(calls)), true);
}

void WebSocketJSExecutor::invokeCallback(const double callbackId, const folly::dynamic &arguments) {
  folly::dynamic jarray = folly::dynamic::array(callbackId, arguments);
  auto calls = Call("invokeCallbackAndReturnFlushedQueue", jarray);
  if (m_delegate && !IsInError())
    m_delegate->callNativeModules(*this, folly::parseJson(std::move(calls)), true);
}

void WebSocketJSExecutor::setGlobalVariable(
    std::string propName,
    std::unique_ptr<const facebook::react::JSBigString> jsonValue) {
  m_injectedObjects[propName] = std::string(jsonValue->c_str());
}

void *WebSocketJSExecutor::getJavaScriptContext() {
  return nullptr;
}

std::string WebSocketJSExecutor::getDescription() {
  return "WebSocketJSExecutor";
}

#ifdef WITH_JSC_MEMORY_PRESSURE
void WebSocketJSExecutor::handleMemoryPressure(int pressureLevel) {}
#endif

void WebSocketJSExecutor::destroy() {
  if (State::Connected == m_state || State::Running == m_state)
    m_socket.Close();

  SetState(State::Disposed);
}

std::string WebSocketJSExecutor::Call(const std::string &methodName, folly::dynamic &arguments) {
  int requestId = ++m_requestId;

  if (!IsRunning()) {
    OnHitError("Executor instance not connected to a WebSocket endpoint.");
    return std::string();
  }

  try {
    folly::dynamic request =
        folly::dynamic::object("id", requestId)("method", methodName)("arguments", std::move(arguments));
    std::string str = folly::toJson(request);
    std::string strReturn = SendMessageAsync(requestId, str).get();
    return strReturn;
  } catch (const std::exception &e) {
    OnHitError(e.what());
    return std::string();
  }
}

void WebSocketJSExecutor::OnHitError(std::string message) {
  if (m_errorCallback != nullptr)
    m_errorCallback(message);
  SetState(State::Error);
}

void WebSocketJSExecutor::OnWaitingForDebugger() {
  SetState(State::Waiting);
  PollPrepareJavaScriptRuntime();
  if (m_waitingForDebuggerCallback != nullptr)
    m_waitingForDebuggerCallback();
}

void WebSocketJSExecutor::OnDebuggerAttach() {
  SetState(State::Running);
  if (m_debuggerAttachCallback != nullptr)
    m_debuggerAttachCallback();
}

winrt::Windows::Foundation::IAsyncAction WebSocketJSExecutor::ConnectAsync(
    const std::string &webSocketServerUrl,
    const std::function<void(std::string)> &errorCallback,
    const std::function<void()> &waitingForDebuggerCallback,
    const std::function<void()> &debuggerAttachCallback) {
  m_errorCallback = errorCallback;
  m_debuggerAttachCallback = debuggerAttachCallback;
  m_waitingForDebuggerCallback = waitingForDebuggerCallback;

  winrt::Windows::Foundation::Uri uri(Microsoft::Common::Unicode::Utf8ToUtf16(webSocketServerUrl));
  co_await m_socket.ConnectAsync(uri);

  SetState(State::Connected);

  if (PrepareJavaScriptRuntime(250)) {
    SetState(State::Running);
  } else {
    m_state = State::Waiting;
    OnWaitingForDebugger();
  }
}

bool WebSocketJSExecutor::PrepareJavaScriptRuntime(int milliseconds) {
  auto timeout = std::chrono::milliseconds(milliseconds);

  int requestId = ++m_requestId;

  folly::dynamic request = folly::dynamic::object("id", requestId)("method", "prepareJSRuntime");
  std::string str = folly::toJson(request);

  return SendMessageAsync(requestId, std::move(str)).wait_for(timeout) == std::future_status::ready;
}

void WebSocketJSExecutor::PollPrepareJavaScriptRuntime() {
  m_messageQueueThread->runOnQueue([this]() {
    auto timeout = std::chrono::milliseconds(750);

    for (uint32_t retries = 20; retries > 0; --retries) {
      if (PrepareJavaScriptRuntime(750)) {
        OnDebuggerAttach();
        return;
      }
    }

    OnHitError("Prepare JS runtime timed out, Executor instance is not connected to a WebSocket endpoint.");
  });
}

std::future<std::string> WebSocketJSExecutor::SendMessageAsync(int requestId, const std::string &message) {
  std::lock_guard<std::mutex> lock(m_lockPromises);
  auto it = m_promises.emplace(requestId, std::promise<std::string>()).first;
  auto future = it->second.get_future();

  if (!IsDisposed()) {
    m_socket.Control().MessageType(winrt::Windows::Networking::Sockets::SocketMessageType::Utf8);

    winrt::array_view<const uint8_t> arr(
        Microsoft::Common::Utilities::CheckedReinterpretCast<const uint8_t *>(message.c_str()),
        Microsoft::Common::Utilities::CheckedReinterpretCast<const uint8_t *>(message.c_str()) + message.length());
    m_socketDataWriter.WriteBytes(arr);
    m_socketDataWriter.StoreAsync();
  } else {
    // Disposed, immediately return empty
    auto promise(std::move(it->second));
    m_promises.erase(it);

    promise.set_value("");
  }

  return future;
}

void WebSocketJSExecutor::OnMessageReceived(const std::string &msg) {
  folly::dynamic parsed = folly::parseJson(msg);
  auto it_parsed = parsed.find("replyID");
  if (it_parsed != parsed.items().end()) {
    int replyId = it_parsed->second.asInt();

    std::lock_guard<std::mutex> lock(m_lockPromises);
    auto it_promise = m_promises.find(replyId);
    if (it_promise != m_promises.end()) {
      auto promise(std::move(it_promise->second));
      m_promises.erase(it_promise);

      it_parsed = parsed.find("result");
      if (it_parsed != parsed.items().end() && it_parsed->second.isString()) {
        std::string result = it_parsed->second.asString();
        promise.set_value(result);
      } else {
        promise.set_value("");
      }
    }
  }
}

} // namespace react::uwp

#pragma warning(pop)
