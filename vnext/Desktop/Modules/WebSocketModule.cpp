// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// clang-format off
#include "pch.h"

#include <WebSocketModule.h>

#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>
#include "../ReactWindowsCore/Utils.h"
#include "Unicode.h"

using namespace facebook::xplat;
using namespace folly;

using Microsoft::Common::Unicode::Utf8ToUtf16;

using std::string;

namespace
{
constexpr char moduleName[] = "WebSocketModule";
} // anonymous namespace

namespace Microsoft::React
{

  WebSocketModule::WebSocketModule() {}

string WebSocketModule::getName()
{
  return moduleName;
}

std::map<string, dynamic> WebSocketModule::getConstants()
{
  return {};
}

std::vector<facebook::xplat::module::CxxModule::Method> WebSocketModule::getMethods()
{
  return
  {
    Method(
      "connect",
      [this](dynamic args) // const string& url, dynamic protocols, dynamic options, int64_t id
      {
        IWebSocketResource::Protocols protocols;
        dynamic protocolsDynamic = jsArgAsDynamic(args, 1);
        if (!protocolsDynamic.empty())
        {
          for (const auto& protocol : protocolsDynamic.items())
          {
            protocols.push_back(protocol.first.getString());
          }
        }

        IWebSocketResource::Options options;
        dynamic optionsDynamic = jsArgAsDynamic(args, 2);
        if (!optionsDynamic.empty() && !optionsDynamic["headers"].empty())
        {
          dynamic headersDynamic = optionsDynamic["headers"];
          for (const auto& header : headersDynamic.items())
          {
            options.emplace(Utf8ToUtf16(header.first.getString()), header.second.getString());
          }
        }

        auto weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 3), jsArgAsString(args, 0));
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->Connect(protocols, options);
        }
      }),
    Method(
      "close",
      [this](dynamic args) // [int64_t code, string reason,] int64_t id
      {
        // See react-native\Libraries\WebSocket\WebSocket.js:_close
        if (args.size() == 3) // WebSocketModule.close(statusCode, closeReason, this._socketId);
        {
          auto weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 2));
          if (auto sharedWs = weakWs.lock())
          {
            sharedWs->Close(static_cast<IWebSocketResource::CloseCode>(jsArgAsInt(args, 0)), jsArgAsString(args, 1));
          }
        }
        else if (args.size() == 1) // WebSocketModule.close(this._socketId);
        {
          auto weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 0));
          if (auto sharedWs = weakWs.lock())
          {
            sharedWs->Close(IWebSocketResource::CloseCode::Normal, {});
          }
        }
        else
        {
          auto errorObj = dynamic::object("id", -1)("message", "Incorrect number of parameters");
          this->SendEvent("websocketFailed", std::move(errorObj));
        }
      }),
    Method(
      "send",
      [this](dynamic args) // const string& message, int64_t id
      {
        auto weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 1));
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->Send(jsArgAsString(args, 0));
        }
      }),
    Method(
      "sendBinary",
      [this](dynamic args) // const string& base64String, int64_t id
      {
        auto weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 1));
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->SendBinary(jsArgAsString(args, 0));
        }
      }),
    Method(
      "ping",
      [this](dynamic args) // int64_t id
      {
        auto weakWs = this->GetOrCreateWebSocket(jsArgAsInt(args, 0));
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->Ping();
        }
      })
  };
} // getMethods

#pragma region private members

void WebSocketModule::SendEvent(string&& eventName, dynamic&& args)
{
  auto instance = this->getInstance().lock();
  if (instance)
  {
    instance->callJSFunction("RCTDeviceEventEmitter", "emit", dynamic::array(std::move(eventName), std::move(args)));
  }
}

std::weak_ptr<IWebSocketResource> WebSocketModule::GetOrCreateWebSocket(int64_t id, string&& url)
{
  if (m_webSockets.find(id) == m_webSockets.end())
  {
    auto ws = IWebSocketResource::Make(std::move(url));
    ws->SetOnError([this, id, ws](const IWebSocketResource::Error& err)
    {
      auto errorObj = dynamic::object("id", id)("message", err.Message);
      this->SendEvent("websocketFailed", std::move(errorObj));
    });
    ws->SetOnConnect([this, id, ws]()
    {
      auto args = dynamic::object("id", id);
      this->SendEvent("websocketOpen", std::move(args));
    });
    ws->SetOnMessage([this, id, ws](size_t length, const string& message)
    {
      auto args = dynamic::object("id", id)("data", message)("type", "text");
      this->SendEvent("websocketMessage", std::move(args));
    });
    ws->SetOnClose([this, id, ws](IWebSocketResource::CloseCode code, const string& reason)
    {
      auto args = dynamic::object("id", id)("code", static_cast<uint16_t>(code))("reason", reason);
      this->SendEvent("websocketClosed", std::move(args));
    });

    m_webSockets.emplace(id, ws);
    return ws;
  }

  return m_webSockets.at(id);
}

#pragma endregion private members

/*extern*/ std::unique_ptr<facebook::xplat::module::CxxModule> CreateWebSocketModule() noexcept
{
  return std::make_unique<WebSocketModule>();
}

} // namespace Microsoft::React
