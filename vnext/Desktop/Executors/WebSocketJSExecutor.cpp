// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "WebSocketJSExecutor.h"

#include <cxxreact/JSBigString.h>
#include <cxxreact/RAMBundleRegistry.h>
#include <folly/dynamic.h>
#include <folly/json.h>

using namespace folly;
using namespace Concurrency;

using std::lock_guard;
using std::move;
using std::mutex;
using std::promise;
using std::shared_ptr;
using std::string;
using std::unique_ptr;

namespace {
const int ConnectTimeoutMilliseconds = 5000;
}

namespace Microsoft::React {

WebSocketJSExecutor::WebSocketJSExecutor(
    const shared_ptr<ExecutorDelegate> &delegate,
    const shared_ptr<MessageQueueThread> &messageQueueThread)
    : m_delegate{delegate}, m_messageQueueThread{messageQueueThread} {}

WebSocketJSExecutor::~WebSocketJSExecutor() {}

#pragma region JSExecutor members

void WebSocketJSExecutor::loadApplicationScript(
    unique_ptr<const JSBigString> script,
#if !defined(OSS_RN)
    uint64_t /*scriptVersion*/,
#endif
    string /*sourceURL*/
#if !defined(OSS_RN)
    ,
    string && /*bytecodeFileName*/
#endif
) {
  int requestId = ++m_requestId;
  promise<string> requestPromise;
  {
    lock_guard<mutex> lock(m_lockPromises);
    m_promises.emplace(requestId, std::move(requestPromise));
  }
  dynamic request =
      dynamic::object("id", requestId)("method", "executeApplicationScript")(
          "url", script->c_str())("inject", m_injectedObjects);

  string str = toJson(request);
  try {
    if (State::Running != m_state)
      throw std::exception(
          "Executor instance not connected to a WebSocket endpoint.");

    SendMessageAsync(requestId, std::move(str)).get();
    if (State::Error == m_state)
      throw std::exception("Failed write.");

    flush();
  }
  // TODO: handle exceptions in a better way
  catch (const std::exception &e) {
    m_errorCallback(e.what());
    SetState(State::Error);
  }
}

void WebSocketJSExecutor::setBundleRegistry(
    unique_ptr<RAMBundleRegistry> bundleRegistry) {}

void WebSocketJSExecutor::registerBundle(
    uint32_t /*bundleId*/,
    const std::string & /*bundlePath*/) {
  // NYI
  std::terminate();
}

void WebSocketJSExecutor::callFunction(
    const string &moduleId,
    const string &methodId,
    const dynamic &arguments) {
  dynamic jArray = dynamic::array(moduleId, methodId, arguments);
  auto calls = Call("callFunctionReturnFlushedQueue", jArray);
  if (m_delegate && !IsInError()) {
    m_delegate->callNativeModules(*this, parseJson(std::move(calls)), true);
  }
}

void WebSocketJSExecutor::invokeCallback(
    const double callbackId,
    const dynamic &arguments) {
  dynamic jArray = dynamic::array(callbackId, arguments);
  auto calls = Call("invokeCallbackAndReturnFlushedQueue", jArray);
  if (m_delegate && !IsInError()) {
    m_delegate->callNativeModules(*this, parseJson(std::move(calls)), true);
  }
}

void WebSocketJSExecutor::setGlobalVariable(
    string propName,
    unique_ptr<const JSBigString> jsonValue) {
  m_injectedObjects[propName] = string(jsonValue->c_str());
}

void *WebSocketJSExecutor::getJavaScriptContext() {
  return nullptr;
}

string WebSocketJSExecutor::getDescription() {
  return s_description;
}

#ifdef WITH_JSC_MEMORY_PRESSURE
void WebSocketJSExecutor::handleMemoryPressure(int pressureLevel) {}
#endif // WITH_JSC_MEMORY_PRESSURE

void WebSocketJSExecutor::destroy() {
  if (State::Connected == m_state || State::Running == m_state)
    m_webSocket->Close(IWebSocket::CloseCode::GoingAway, "Disposed");

  SetState(State::Disposed);
}

#pragma endregion

std::future<bool> WebSocketJSExecutor::ConnectAsync(
    const string &webSocketServerUrl,
    const std::function<void(string)> &errorCallback) {
  promise<bool> resultPromise;
  m_errorCallback = std::move(errorCallback);

  m_webSocket = IWebSocket::Make(webSocketServerUrl);
  m_webSocket->SetOnMessage([this](size_t /*length*/, const string &message) {
    this->OnMessageReceived(message);
  });
  m_webSocket->SetOnError(
      [this](const IWebSocket::Error &err) { this->SetState(State::Error); });

  try {
    promise<void> connectPromise;
    if (!IsConnected()) {
      m_webSocket->SetOnConnect(
          [&connectPromise]() { connectPromise.set_value(); });

      m_webSocket->Connect({} /*protocols*/, {} /*options*/);
    } else {
      connectPromise.set_value();
    }

    auto status = connectPromise.get_future().wait_for(
        std::chrono::milliseconds(ConnectTimeoutMilliseconds));
    if (std::future_status::ready != status) {
      m_errorCallback("Timeout: Failed to connect to the dev server");
      m_webSocket->Close(IWebSocket::CloseCode::ProtocolError, "Timed out");

      resultPromise.set_value(false);
      return resultPromise.get_future();
      // return false;//TODO: Return?
    }

    SetState(State::Connected);
    PrepareJavaScriptRuntime();
    SetState(State::Running);

    resultPromise.set_value(IsRunning());
    return resultPromise.get_future(); // TODO: Return bool instead?
  } catch (const std::exception &) {
    m_errorCallback(
        IsConnected() ? "Timeout: preparing JS runtime"
                      : "Timeout: Failed to connect to the dev server");

    try {
      m_webSocket->Close(IWebSocket::CloseCode::ProtocolError, "Timed out");
    } catch (const std::exception &) {
      // Nothing left to do. Aborting operation.
    }
  } // try m_webSocket->Connect()

  // Default: not connected.
  resultPromise.set_value(false);
  return resultPromise.get_future();
}

#pragma region private members

/*static*/ const string WebSocketJSExecutor::s_description =
    "WebSocketJSExecutor";

void WebSocketJSExecutor::PrepareJavaScriptRuntime() {
  promise<string> requestPromise;
  int requestId = ++m_requestId;
  {
    lock_guard<mutex> lock(m_lockPromises);
    m_promises.emplace(requestId, std::move(requestPromise));
  }

  dynamic request =
      dynamic::object("id", requestId)("method", "prepareJSRuntime");
  string str = toJson(request);

  SendMessageAsync(requestId, std::move(str)).get();
  if (State::Error == m_state)
    throw std::exception("Failed write.");
}

string WebSocketJSExecutor::Call(const string &methodName, dynamic &arguments) {
  int requestId = ++m_requestId;
  promise<string> requestPromise;
  {
    lock_guard<mutex> lock(m_lockPromises);
    m_promises.emplace(requestId, std::move(requestPromise));
  }

  dynamic request = dynamic::object("id", requestId)("method", methodName)(
      "arguments", std::move(arguments));
  string str = toJson(request);

  try {
    if (State::Running != m_state)
      throw std::exception(
          "Executor instance not connected to a WebSocket endpoint.");

    auto message{SendMessageAsync(requestId, std::move(str)).get()};
    if (State::Error == m_state)
      throw std::exception("Error on read or write.");

    return message;
  }
  // TODO: Handle exceptions in a bettery way
  catch (const std::exception &e) {
    m_errorCallback(e.what());
    SetState(State::Error);

    return e.what();
  }
}

std::future<string> WebSocketJSExecutor::SendMessageAsync(
    int requestId,
    string &&message) {
  if (!IsDisposed()) {
    m_webSocket->Send(std::move(message));
    auto itr = m_promises.find(requestId);
    if (itr != m_promises.end())
      return itr->second.get_future();
  } else {
    lock_guard<mutex> lock(m_lockPromises);
    auto itr = m_promises.find(requestId);
    if (itr != m_promises.end()) {
      itr->second.set_value("");
      return itr->second.get_future();
    }
  }

  promise<string> emptyPromise;
  emptyPromise.set_value("");

  return emptyPromise.get_future();
}

void WebSocketJSExecutor::OnMessageReceived(const string &msg) {
  dynamic parsed = parseJson(msg);
  auto it_parsed = parsed.find("replyID");
  auto it_end = parsed.items().end();
  if (it_parsed != it_end) {
    auto replyId = it_parsed->second.asInt();

    lock_guard<mutex> lock(m_lockPromises);
    auto it_promise = m_promises.find(static_cast<int>(replyId));
    if (it_promise != m_promises.end()) {
      it_parsed = parsed.find("result");
      if (it_parsed != it_end && it_parsed->second != nullptr) {
        auto result = it_parsed->second.asString();
        it_promise->second.set_value(result);
      } else {
        it_promise->second.set_value("");
      }
    } // it_promise != m_promises.end

  } // it_parsed != it_end

  // If an error is found in the payload, propagate and return;
  auto it_error = parsed.find("error");
  auto it_status = parsed.find("status");
  if (it_error != it_end) {
    m_errorCallback(it_error->second.asString());
  } else if (it_status != it_end) {
    // Handle HTTP error status codes (4XX: client, 5XX: server).
    // See
    // http://www.iana.org/assignments/http-status-codes/http-status-codes.xhtml
    auto httpStatus = it_status->second.asInt();
    if (httpStatus >= 400 || httpStatus <= 599)
      m_errorCallback(it_status->second.asString());
  }
}

void WebSocketJSExecutor::flush() {
  dynamic jArray = dynamic::array();
  auto calls = Call("flushedQueue", jArray);
  if (m_delegate && !IsInError()) {
    m_delegate->callNativeModules(*this, parseJson(std::move(calls)), true);
  }
}

void WebSocketJSExecutor::SetState(State state) noexcept {
  m_state = state;
}

bool WebSocketJSExecutor::IsConnected() const noexcept {
  return m_state == State::Connected;
}

bool WebSocketJSExecutor::IsDisposed() const noexcept {
  return m_state == State::Disposed;
}

bool WebSocketJSExecutor::IsRunning() const noexcept {
  return m_state == State::Running;
}

bool WebSocketJSExecutor::IsInError() const noexcept {
  return m_state == State::Error;
}

#pragma endregion private members

} // namespace Microsoft::React
