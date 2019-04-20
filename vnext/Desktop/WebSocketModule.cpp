// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <WebSocketModule.h>

#include "UnicodeConversion.h"
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>
#include "../ReactWindowsCore/Utils.h"

using namespace facebook::xplat;
using namespace folly;

using std::string;

namespace Microsoft {
namespace React {

WebSocketModule::WebSocketModule()
{
}

WebSocketModule::~WebSocketModule()
{
  for (auto& pair : m_webSockets)
  {
    // Best-effort attempt to close open websockets.
    if (pair.second->GetReadyState() != IWebSocket::ReadyState::Closed && pair.second->GetReadyState() != IWebSocket::ReadyState::Closing)
      pair.second->Close(IWebSocket::CloseCode::GoingAway, {});
  }

  // Aborts any pending websocket async tasks.
  m_webSockets.clear();
}

string WebSocketModule::getName()
{
  return "WebSocketModule";
}

std::map<string, dynamic> WebSocketModule::getConstants()
{
  return {};
}

std::vector<facebook::xplat::module::CxxModule::Method> WebSocketModule::getMethods()
{
  return
  {
    Method("connect", [this](dynamic args) // const string& url, folly::dynamic protocols, folly::dynamic options, int64_t id
      {
        IWebSocket::Protocols protocols;
        dynamic protocolsDynamic = jsArgAsDynamic(args, 1);
        if (!protocolsDynamic.empty())
        {
          for (const auto& protocol : protocolsDynamic.items())
          {
            protocols.push_back(protocol.first.getString());
          }
        }

        IWebSocket::Options options;
        dynamic optionsDynamic = jsArgAsDynamic(args, 2);
        if (!optionsDynamic.empty() && !optionsDynamic["headers"].empty())
        {
          dynamic headersDynamic = optionsDynamic["headers"];
          for (const auto& header : headersDynamic.items())
          {
            options.emplace(facebook::react::UnicodeConversion::Utf8ToUtf16(header.first.getString()), header.second.getString());
          }
        }

        this->GetWebSocket(jsArgAsInt(args, 3), jsArgAsString(args, 0))->Connect(protocols, options);
      }),
    Method("close", [this](dynamic args) // [int64_t code, string reason,] int64_t id
      {
        // See react-native\Libraries\WebSocket\WebSocket.js:_close
        if (args.size() == 3)      // WebSocketModule.close(statusCode, closeReason, this._socketId);
          this->GetWebSocket(jsArgAsInt(args, 2))->Close(static_cast<IWebSocket::CloseCode>(jsArgAsInt(args, 0)), jsArgAsString(args, 1));
        else if (args.size() == 1) // WebSocketModule.close(this._socketId);
          this->GetWebSocket(jsArgAsInt(args, 0))->Close(IWebSocket::CloseCode::Normal, {});
        else
        {
          auto errorObj = dynamic::object("id", -1)("message", "Incorrect number of parameters");
          this->SendEvent("websocketFailed", std::move(errorObj));
        }
      }),
    Method("send", [this](dynamic args) // const string& message, int64_t id
      {
        this->GetWebSocket(jsArgAsInt(args, 1))->Send(jsArgAsString(args, 0));
      }),
    Method("sendBinary", [this](dynamic args) // const string& base64String, int64_t id
      {
        this->GetWebSocket(jsArgAsInt(args, 1))->SendBinary(jsArgAsString(args, 0));
      }),
    Method("ping", [this](dynamic args) // int64_t id
      {
        this->GetWebSocket(jsArgAsInt(args, 0))->Ping();
      })
  };
}//getMethods

#pragma region private members

void WebSocketModule::SendEvent(string&& eventName, dynamic&& args)
{
  auto instance = this->getInstance().lock();
  if (instance)
    instance->callJSFunction("RCTDeviceEventEmitter", "emit", dynamic::array(std::move(eventName), std::move(args)));
}

IWebSocket* WebSocketModule::GetWebSocket(int64_t id, string&& url)
{
  IWebSocket* ptr = nullptr;
  if (m_webSockets.find(id) == m_webSockets.end())
  {
    auto ws = IWebSocket::Make(std::move(url));
    ws->SetOnError([this, id](const IWebSocket::Error& err)
    {
      auto errorObj = dynamic::object("id", id)("message", err.Message);
      this->SendEvent("websocketFailed", std::move(errorObj));
    });
    ws->SetOnConnect([this, id]()
    {
      dynamic args = dynamic::object("id", id);
      this->SendEvent("websocketOpen", std::move(args));
    });
    ws->SetOnMessage([this, id](size_t length, const string& message)
    {
      dynamic args = dynamic::object("id", id)("data", message)("type", "text");
      this->SendEvent("websocketMessage", std::move(args));
    });
    ws->SetOnClose([this, id](IWebSocket::CloseCode code, const string& reason)
    {
      dynamic args = dynamic::object("id", id)("code", static_cast<uint16_t>(code))("reason", reason);
      this->SendEvent("websocketClosed", std::move(args));
    });

    ptr = ws.get();
    m_webSockets.emplace(id, std::move(ws));
  }
  else
  {
    ptr = m_webSockets.at(id).get();
  }

  return ptr;
}

#pragma endregion // private members

std::unique_ptr<facebook::xplat::module::CxxModule> CreateWebSocketModule() noexcept
{
	return std::make_unique<WebSocketModule>();
}

} } // Microsoft::React
