// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "WebSocketJSExecutor.h"

#include <cxxreact/JSBigString.h>
#include <cxxreact/RAMBundleRegistry.h>
#include <folly/dynamic.h>
#include <folly/json.h>
#include <agents.h>

using namespace folly;
using namespace Concurrency;

using std::lock_guard;
using std::move;
using std::mutex;
using std::shared_ptr;
using std::string;
using std::unique_ptr;

namespace {
const int ConnectTimeoutMilliseconds = 5000;
}

namespace facebook {
namespace react {

WebSocketJSExecutor::WebSocketJSExecutor(const shared_ptr<ExecutorDelegate>& delegate, const shared_ptr<MessageQueueThread>& messageQueueThread)
  : m_delegate { delegate }
  , m_messageQueueThread { messageQueueThread }
{
}

WebSocketJSExecutor::~WebSocketJSExecutor()
{
}

#pragma region JSExecutor members

void WebSocketJSExecutor::loadApplicationScript(unique_ptr<const JSBigString> script,
#if !defined(OSS_RN)
  uint64_t /*scriptVersion*/,
#endif
  string /*sourceURL*/
#if !defined(OSS_RN)
  , string&& /*bytecodeFileName*/
#endif
)
{
  int requestId = ++m_requestId;
  task_completion_event<string> callback;
  {
    lock_guard<mutex> lock(m_lockCallbacks);
    m_callbacks.emplace(requestId, callback);
  }
  dynamic request = dynamic::object
  ("id", requestId)
    ("method", "executeApplicationScript")
    ("url", script->c_str())
    ("inject", m_injectedObjects);

  string str = toJson(request);
  try
  {
    if (State::Running != m_state)
      throw std::exception("Executor instance not connected to a WebSocket endpoint.");

    SendMessageAsync(requestId, move(str)).get();
    if (State::Error == m_state)
      throw std::exception("Failed write.");

    flush();
  }
  //TODO: handle exceptions in a better way
  catch (const std::exception& e)
  {
    m_errorCallback(e.what());
    SetState(State::Error);
  }
}

void WebSocketJSExecutor::setBundleRegistry(unique_ptr<RAMBundleRegistry> bundleRegistry)
{
}

void WebSocketJSExecutor::registerBundle(uint32_t /*bundleId*/, const std::string& /*bundlePath*/)
{
  // NYI
  std::terminate();
}

void WebSocketJSExecutor::callFunction(const string& moduleId, const string& methodId, const dynamic& arguments)
{
  dynamic jArray = dynamic::array(moduleId, methodId, arguments);
  auto calls = Call("callFunctionReturnFlushedQueue", jArray);
  if (m_delegate && !IsInError())
  {
    m_delegate->callNativeModules(*this, parseJson(move(calls)), true);
  }
}

void WebSocketJSExecutor::invokeCallback(const double callbackId, const dynamic& arguments)
{
  dynamic jArray = dynamic::array(callbackId, arguments);
  auto calls = Call("invokeCallbackAndReturnFlushedQueue", jArray);
  if (m_delegate && !IsInError())
  {
    m_delegate->callNativeModules(*this, parseJson(move(calls)), true);
  }
}

void WebSocketJSExecutor::setGlobalVariable(string propName, unique_ptr<const JSBigString> jsonValue)
{
  m_injectedObjects[propName] = string(jsonValue->c_str());
}

void* WebSocketJSExecutor::getJavaScriptContext()
{
  return nullptr;
}

string WebSocketJSExecutor::getDescription()
{
  return s_description;
}

#ifdef WITH_JSC_MEMORY_PRESSURE
void WebSocketJSExecutor::handleMemoryPressure(int pressureLevel)
{
}
#endif // WITH_JSC_MEMORY_PRESSURE

void WebSocketJSExecutor::destroy()
{
  if (State::Connected == m_state || State::Running == m_state)
    m_webSocket->Close(/*closeCode*/ IWebSocket::CloseCode::Normal, "Disposed");

  SetState(State::Disposed);
}

#pragma endregion

template <typename Func>
auto create_delayed_task(std::chrono::milliseconds delay, Func func, concurrency::cancellation_token token = concurrency::cancellation_token::none()) -> decltype(create_task(func))
{
  concurrency::task_completion_event<void> tce;

  auto pCallback = new concurrency::call<int>([tce](int)
  {
    tce.set();
  });
  auto pTimer = new concurrency::timer<int>(static_cast<int>(delay.count()), 0, pCallback, false);

  pTimer->start();
  token.register_callback([tce]()
  {
    tce.set();
  });

  return create_task(tce).then([pCallback, pTimer]()
  {
    delete pCallback;
    delete pTimer;
  }).then(func, token);
}

task<bool> WebSocketJSExecutor::ConnectAsync(const string& webSocketServerUrl, const std::function<void(string)>& errorCallback)
{
  m_errorCallback = move(errorCallback);
  auto t = task_from_result();

  m_webSocket = IWebSocket::Make(webSocketServerUrl);
  m_webSocket->SetOnMessage([this](size_t length, const string& message)
  {
    this->OnMessageReceived(message);
  });
  m_webSocket->SetOnError([this](const IWebSocket::Error& err)
  {
    this->SetState(State::Error);
  });

  return t.then([=]() -> bool
  {
    try
    {
      cancellation_token_source timer_cts;
      auto timeoutT = create_delayed_task(std::chrono::milliseconds(ConnectTimeoutMilliseconds), [=]() -> string
      {
        throw std::runtime_error("timeout");
      }, timer_cts.get_token());

      if (!IsConnected())
      {
        m_webSocket->SetOnConnect([this, &timer_cts]()
        {
          this->SetState(State::Connected);
          timer_cts.cancel();
        });

        m_webSocket->Connect({} /*protocols*/, {} /*options*/);
      }

      auto status = timeoutT.wait();
      if (status != canceled)
        throw new std::exception("Timeout");

      if (IsConnected())
      {
        PrepareJavaScriptRuntime();
        SetState(State::Running);
      }

      return IsRunning();
    }
    catch (const std::exception&)
    {
      m_errorCallback(IsConnected() ? "Timeout: preparing JS runtime" : "Timeout: Failed to connect to dev server");
      SetState(State::Error);

      try
      {
        //TODO: Research appropriate close_code value.
        m_webSocket->Close(IWebSocket::CloseCode::GoingAway, "Timed out.");
      }
      catch (const std::exception&)
      {
        // Nothing left to do. Aborting operation.
      }

      return false;
    }// try m_webSocket->Connect()
  });// t.then [=]() -> bool
}

#pragma region private members

/*static*/ const string WebSocketJSExecutor::s_description = "WebSocketJSExecutor";

void WebSocketJSExecutor::PrepareJavaScriptRuntime()
{
  task_completion_event<string> callback;
  int requestId = ++m_requestId;
  {
    lock_guard<mutex> lock(m_lockCallbacks);
    m_callbacks.emplace(requestId, callback);
  }

  dynamic request = dynamic::object
    ("id", requestId)
    ("method", "prepareJSRuntime");
  string str = toJson(request);

  SendMessageAsync(requestId, move(str)).get();
  if (State::Error == m_state)
    throw std::exception("Failed write.");
}

string WebSocketJSExecutor::Call(const string& methodName, dynamic& arguments)
{
  int requestId = ++m_requestId;
  task_completion_event<string> callback;
  {
    lock_guard<mutex> lock(m_lockCallbacks);
    m_callbacks.emplace(requestId, callback);
  }

  dynamic request = dynamic::object
    ("id", requestId)
    ("method", methodName)
    ("arguments", move(arguments));
  string str = toJson(request);

  try
  {
    if (State::Running != m_state)
      throw std::exception("Executor instance not connected to a WebSocket endpoint.");

    auto message { SendMessageAsync(requestId, move(str)).get() };
    if (State::Error == m_state)
      throw std::exception("Error on read or write.");

    return message;
  }
  //TODO: Handle exceptions in a bettery way
  catch (const std::exception& e)
  {
    m_errorCallback(e.what());
    SetState(State::Error);

    return e.what();
  }
}

task<string> WebSocketJSExecutor::SendMessageAsync(int requestId, const string&& message)
{
  if (!IsDisposed())
  {
    m_webSocket->Send(move(message));

    auto itr = m_callbacks.find(requestId);
    if (itr != std::end(m_callbacks))
      return task<string>(itr->second);
  }
  else
  {
    lock_guard<mutex> lock(m_lockCallbacks);
    auto itr = m_callbacks.find(requestId);
    if (itr != m_callbacks.end())
    {
      itr->second.set("");
      return task<string>(itr->second);
    }
  }

  return task<string>([]()
  {
    return string();
  });
}

void WebSocketJSExecutor::OnMessageReceived(const string& msg)
{
  dynamic parsed = parseJson(msg);
  auto it_parsed = parsed.find("replyID");
  auto it_end = parsed.items().end();
  if (it_parsed != it_end)
  {
    auto replyId = it_parsed->second.getInt();

    lock_guard<mutex> lock(m_lockCallbacks);
    auto it_callback = m_callbacks.find(static_cast<int>(replyId));
    if (it_callback != m_callbacks.end())
    {
      it_parsed = parsed.find("result");
      if (it_parsed != it_end && it_parsed->second != nullptr)
      {
        string result = it_parsed->second.asString();
        it_callback->second.set(result);
      }
      else
      {
        it_callback->second.set("");
      }
    } // it_callback != m_callbacks.end
  } // it_parsed != it_end

  // If an error is found in the payload, propagate and return;
  auto it_error = parsed.find("error");
  auto it_status = parsed.find("status");
  if (it_error != it_end)
  {
    m_errorCallback(it_error->second.asString());
  }
  else if (it_status != it_end)
  {
    // Handle HTTP error status codes (4XX: client, 5XX: server).
    // See http://www.iana.org/assignments/http-status-codes/http-status-codes.xhtml
    auto httpStatus = it_status->second.asInt();
    if (httpStatus >= 400 || httpStatus <= 599)
      m_errorCallback(it_status->second.asString());
  }
}

void WebSocketJSExecutor::flush()
{
  dynamic jArray = dynamic::array();
  auto calls = Call("flushedQueue", jArray);
  if (m_delegate && !IsInError())
  {
    m_delegate->callNativeModules(*this, parseJson(move(calls)), true);
  }
}

void WebSocketJSExecutor::SetState(State state) noexcept
{
  m_state = state;
}

bool WebSocketJSExecutor::IsConnected() const noexcept
{
  return m_state == State::Connected;
}

bool WebSocketJSExecutor::IsDisposed() const noexcept
{
  return m_state == State::Disposed;
}

bool WebSocketJSExecutor::IsRunning() const noexcept
{
  return m_state == State::Running;
}

bool WebSocketJSExecutor::IsInError() const noexcept
{
  return m_state == State::Error;
}

#pragma endregion // private members

}} // namespace facebook::react
