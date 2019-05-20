// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "WebSocketModuleUwp.h"
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Storage.Streams.h>
#include "utilities.h"
#include "unicode.h"
#include <future>

#pragma warning(push)
#pragma warning(disable : 4146)
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>
#pragma warning(pop)

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize( "", off )
#endif

namespace winrt {
using namespace Windows::Networking::Sockets;
}

namespace react { namespace uwp {

void OutputDebugString(const char* format, winrt::hresult_error const& e)
{
  char buffer[1024];
  _snprintf_s(buffer, _countof(buffer), _TRUNCATE, format, e.code(), e.message().c_str());
  OutputDebugStringA(buffer);
}

//
// WebSocketModule::WebSocket
//

class WebSocketModule::WebSocket
{
public:
  WebSocket(WebSocketModule* parent)
    : m_parent(parent)
  {
  }

  void Disconnect()
  {
    m_parent = nullptr;
  }

  void connect(const std::string& url, folly::dynamic /* @Nullable final ReadableArray */ protocols, folly::dynamic /* @Nullable final ReadableArray */ headers, int64_t id);
  void close(int64_t id);
  void send(const std::string& message, int64_t id);
  void sendBinary(const std::string& base64String, int64_t id);
  void ping(int64_t id);

  void onError(int64_t id, winrt::hresult_error const & e);
  void sendEvent(std::string&& eventName, folly::dynamic&& parameters);

private:
  WebSocketModule* m_parent;
  std::map<int64_t, winrt::MessageWebSocket> m_ws_clients;
  std::map<int64_t, winrt::Windows::Storage::Streams::DataWriter> m_dataWriters;
  std::map<int64_t, winrt::MessageWebSocket::MessageReceived_revoker> m_msgReceiveds;
};

// #define DEFAULT_WEBSOCKET_EXCEPTIONS
#ifndef DEFAULT_WEBSOCKET_EXCEPTIONS
// This assumes RS5 winrtcpp SDK.  Set the define if winrtcpp internals change and break this
// lessthrow - based on await_adapter but doesn't throw from await_resume for APIs that fail in normal usage.
// When we can assume Windows 10 20H1 a TryConnectAsync API should exist that doesn't throw
template <typename Async>
struct lessthrow_await_adapter
{
    Async const& async;

    bool await_ready() const
    {
        return async.Status() == winrt::Windows::Foundation::AsyncStatus::Completed;
    }

    void await_suspend(std::experimental::coroutine_handle<> handle) const
    {
        // TODO: Update when RS5 final SDK is in use everywhere
        //auto context = winrt::capture<winrt::impl::IContextCallback>(WINRT_CoGetObjectContext);
        winrt::com_ptr<winrt::impl::IContextCallback> context;
        winrt::check_hresult(WINRT_CoGetObjectContext(winrt::guid_of<winrt::impl::IContextCallback>(), context.put_void()));

        async.Completed([handle, context = std::move(context)](auto const&, winrt::Windows::Foundation::AsyncStatus)
        {
            winrt::impl::com_callback_args args{};
            args.data = handle.address();

            auto callback = [](winrt::impl::com_callback_args* args) noexcept -> int32_t
            {
                std::experimental::coroutine_handle<>::from_address(args->data)();
                return S_OK;
            };

            winrt::check_hresult(context->ContextCallback(callback, &args, winrt::guid_of<winrt::impl::ICallbackWithNoReentrancyToApplicationSTA>(), 5, nullptr));
        });
    }

    void await_resume() const
    {
      // Don't check for a failure result here
      return;
    }
};
#endif

std::future<HRESULT> Connect(winrt::Windows::Networking::Sockets::MessageWebSocket& socket, winrt::Windows::Foundation::Uri& uri)
{
  auto async = socket.ConnectAsync(uri);
#ifdef DEFAULT_WEBSOCKET_EXCEPTIONS
  co_await async;
#else
  co_await lessthrow_await_adapter<winrt::Windows::Foundation::IAsyncAction>{async};
#endif
  return async.ErrorCode();
}

void WebSocketModule::WebSocket::connect(const std::string& url, folly::dynamic /* @Nullable final ReadableArray */ protocols, folly::dynamic /* @Nullable final ReadableArray */ options, int64_t id)
{
  m_ws_clients.emplace(id, winrt::MessageWebSocket());
  auto socket = m_ws_clients[id];
  m_dataWriters.emplace(id, winrt::Windows::Storage::Streams::DataWriter(socket.OutputStream()));

  if (!options.empty() && !options["headers"].empty())
  {
    auto headers = options["headers"];
    for (auto& header : headers.items())
    {
      auto name = header.first.getString();
      auto value = header.second.getString();

      socket.SetRequestHeader(facebook::react::unicode::utf8ToUtf16(name), facebook::react::unicode::utf8ToUtf16(value));
    }
  }

  if (!protocols.empty())
  {
    winrt::Windows::Foundation::Collections::IVector<winrt::hstring> supportedProtocols = socket.Control().SupportedProtocols();
    for (auto& protocol : protocols)
    {
      auto protocolStr = protocol.getString();
      supportedProtocols.Append(facebook::react::unicode::utf8ToUtf16(protocolStr));
    }
  }

  m_msgReceiveds[id] = socket.MessageReceived(winrt::auto_revoke, [this, id](auto&&, auto&& args)
  {
    try
    {
      std::string response;
      winrt::Windows::Storage::Streams::DataReader reader = args.GetDataReader();

      uint32_t len = reader.UnconsumedBufferLength();
      if (args.MessageType() == winrt::SocketMessageType::Utf8)
      {
        reader.UnicodeEncoding(winrt::Windows::Storage::Streams::UnicodeEncoding::Utf8);
        std::vector<uint8_t> data(len);
        reader.ReadBytes(data);

        response = std::string(facebook::react::utilities::checkedReinterpretCast<char*>(data.data()), data.size());
      }
      else
      {
        auto buffer = reader.ReadBuffer(len);
        winrt::hstring data = winrt::Windows::Security::Cryptography::CryptographicBuffer::EncodeToBase64String(buffer);

        response = facebook::react::unicode::utf16ToUtf8(std::wstring_view(data));
      }

      folly::dynamic params = folly::dynamic::object("id", id)("data", response)("type", args.MessageType() == winrt::SocketMessageType::Utf8 ? "text" : "binary");
      sendEvent("websocketMessage", std::move(params));
    }
    catch (winrt::hresult_error const& e)
    {
      OutputDebugString("Read failed (0x%8X) %ls\n", e);
      onError(id, e);
    }
  });

  winrt::Windows::Foundation::Uri uri(facebook::react::unicode::utf8ToUtf16(url));

  HRESULT hr = S_OK;
  try
  {
    auto async = Connect(socket, uri);
    winrt::hresult hr = async.get();
    if (SUCCEEDED(hr))
    {
      folly::dynamic params = folly::dynamic::object("id", id);
      sendEvent("websocketOpen", std::move(params));
    }
  }
  catch (winrt::hresult_error const & e)
  {
    hr = e.code();
  }

  if (!SUCCEEDED(hr))
  {
    winrt::hresult_error e{ hr };
    OutputDebugString("WebSocket.connect failed (0x%8X) %ls\n", e);
    onError(id, e);
  }

}

void WebSocketModule::WebSocket::close(int64_t id)
{
  auto socket = m_ws_clients[id];
  socket.Close();
}

void WebSocketModule::WebSocket::send(const std::string& message, int64_t id)
{
  try
  {
    auto socket = m_ws_clients[id];
    auto dataWriter = m_dataWriters[id];
    socket.Control().MessageType(winrt::SocketMessageType::Utf8);

    winrt::array_view<const uint8_t> arr(facebook::react::utilities::checkedReinterpretCast<const uint8_t*>(message.c_str()), facebook::react::utilities::checkedReinterpretCast<const uint8_t*>(message.c_str()) + message.length());
    dataWriter.WriteBytes(arr);
    dataWriter.StoreAsync();
  }
  catch (winrt::hresult_error const &e)
  {
    OutputDebugString("WebSocket.send failed (0x%8X) %ls\n", e);
    onError(id, e);
  }
}

void WebSocketModule::WebSocket::sendBinary(const std::string& base64String, int64_t id)
{
  try
  {
    auto socket = m_ws_clients[id];
    auto dataWriter = m_dataWriters[id];
    socket.Control().MessageType(winrt::SocketMessageType::Binary);

    auto buffer = winrt::Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(facebook::react::unicode::utf8ToUtf16(base64String));
    dataWriter.WriteBuffer(buffer);
    dataWriter.StoreAsync();
  }
  catch (winrt::hresult_error const &e)
  {
    OutputDebugString("WebSocket.sendBinary failed (0x%8X) %ls\n", e);
    onError(id, e);
  }
}

void WebSocketModule::WebSocket::ping(int64_t id)
{
  try
  {
    auto socket = m_ws_clients[id];
    auto dataWriter = m_dataWriters[id];
    socket.Control().MessageType(winrt::SocketMessageType::Utf8);

    std::string s("");
    winrt::array_view<const uint8_t> arr(facebook::react::utilities::checkedReinterpretCast<const uint8_t*>(s.c_str()), facebook::react::utilities::checkedReinterpretCast<const uint8_t*>(s.c_str()) + s.length());
    dataWriter.WriteBytes(arr);
    dataWriter.StoreAsync();
  }
  catch (winrt::hresult_error const &e)
  {
    OutputDebugString("WebSocket.ping failed (0x%8X) %ls\n", e);
    onError(id, e);
  }
}

void WebSocketModule::WebSocket::onError(int64_t id, winrt::hresult_error const & e)
{
  folly::dynamic errorObj = folly::dynamic::object("id", id)("message", static_cast<int32_t>(e.code()));
  sendEvent("websocketFailed", std::move(errorObj));
}

void WebSocketModule::WebSocket::sendEvent(std::string&& eventName, folly::dynamic&& parameters)
{
  if (!m_parent)
    return;

  auto instance = m_parent->getInstance().lock();
  if (instance)
    instance->callJSFunction("RCTDeviceEventEmitter", "emit", folly::dynamic::array(eventName, std::move(parameters)));
}


//
// WebSocketModule
//

const char* WebSocketModule::name = "WebSocketModule";

WebSocketModule::WebSocketModule()
  : m_webSocket(std::make_shared<WebSocket>(this))
{
}

WebSocketModule::~WebSocketModule()
{
  m_webSocket->Disconnect();
}

std::string WebSocketModule::getName()
{
  return name;
}

std::map<std::string, folly::dynamic> WebSocketModule::getConstants()
{
  std::map<std::string, folly::dynamic> constants {
    {
    }
  };

  return constants;
}

auto WebSocketModule::getMethods() -> std::vector<Method>
{
  std::shared_ptr<WebSocket> webSocket(m_webSocket);
  return
  {
    Method("connect", [webSocket](folly::dynamic args) // const std::string& url, folly::dynamic /* @Nullable final ReadableArray */ protocols, folly::dynamic /* @Nullable final ReadableArray */ headers, int64_t id
    {
      webSocket->connect(facebook::xplat::jsArgAsString(args, 0), facebook::xplat::jsArgAsDynamic(args, 1), facebook::xplat::jsArgAsDynamic(args, 2), facebook::xplat::jsArgAsInt(args, 3));
    }),
      Method("close", [webSocket](folly::dynamic args) // int64_t id
    {
      webSocket->close(facebook::xplat::jsArgAsInt(args, 0));
    }),
      Method("send", [webSocket](folly::dynamic args) // const std::string& message, int64_t id
    {
      webSocket->send(facebook::xplat::jsArgAsString(args, 0), facebook::xplat::jsArgAsInt(args, 1));
    }),
      Method("sendBinary", [webSocket](folly::dynamic args) // const std::string& base64String, int64_t id
    {
      webSocket->sendBinary(facebook::xplat::jsArgAsString(args, 0), facebook::xplat::jsArgAsInt(args, 1));
    }),
      Method("ping", [webSocket](folly::dynamic args) // int64_t id
    {
      webSocket->ping(facebook::xplat::jsArgAsInt(args, 0));
    }),
  };
}

} }
