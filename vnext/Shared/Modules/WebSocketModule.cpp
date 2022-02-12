// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Modules/WebSocketModule.h>

#include <Utils.h>
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>
#include "Unicode.h"

// Standard Libriary
#include <iomanip>

using namespace facebook::xplat;

using facebook::react::Instance;
using folly::dynamic;

using Microsoft::Common::Unicode::Utf16ToUtf8;
using Microsoft::Common::Unicode::Utf8ToUtf16;

using std::shared_ptr;
using std::string;
using std::weak_ptr;

namespace {
using Microsoft::React::IWebSocketResource;
using Microsoft::React::WebSocketModule;

constexpr char moduleName[] = "WebSocketModule";

static void SendEvent(weak_ptr<Instance> weakInstance, string &&eventName, dynamic &&args) {
  if (auto instance = weakInstance.lock()) {
    instance->callJSFunction("RCTDeviceEventEmitter", "emit", dynamic::array(std::move(eventName), std::move(args)));
  }
}

static shared_ptr<IWebSocketResource>
GetOrCreateWebSocket(int64_t id, string &&url, weak_ptr<WebSocketModule::SharedState> weakState) {
  auto state = weakState.lock();
  if (!state) {
    return nullptr;
  }

  auto itr = state->ResourceMap.find(id);
  if (itr == state->ResourceMap.end()) {
    if (!state->Module) {
      return nullptr;
    }
    auto weakInstance = state->Module->getInstance();

    shared_ptr<IWebSocketResource> ws;
    try {
      ws = state->ResourceFactory(std::move(url));
    } catch (const winrt::hresult_error &e) {
      std::stringstream ss;
      ss << "[" << std::hex << std::showbase << std::setw(8) << static_cast<uint32_t>(e.code()) << "] "
         << winrt::to_string(e.message());

      SendEvent(weakInstance, "webSocketFailed", dynamic::object("id", id)("message", std::move(ss.str())));

      return nullptr;
    } catch (const std::exception &e) {
      SendEvent(weakInstance, "webSocketFailed", dynamic::object("id", id)("message", e.what()));

      return nullptr;
    } catch (...) {
      SendEvent(
          weakInstance,
          "webSocketFailed",
          dynamic::object("id", id)("message", "Unidentified error creating IWebSocketResource"));

      return nullptr;
    }

    ws->SetOnError([id, weakInstance](const IWebSocketResource::Error &err) {
      auto strongInstance = weakInstance.lock();
      if (!strongInstance)
        return;

      auto errorObj = dynamic::object("id", id)("message", err.Message);
      SendEvent(weakInstance, "websocketFailed", std::move(errorObj));
    });
    ws->SetOnConnect([id, weakInstance]() {
      auto strongInstance = weakInstance.lock();
      if (!strongInstance)
        return;

      auto args = dynamic::object("id", id);
      SendEvent(weakInstance, "websocketOpen", std::move(args));
    });
    ws->SetOnMessage([id, weakInstance](size_t length, const string &message, bool isBinary) {
      auto strongInstance = weakInstance.lock();
      if (!strongInstance)
        return;

      auto args = dynamic::object("id", id)("data", message)("type", isBinary ? "binary" : "text");
      SendEvent(weakInstance, "websocketMessage", std::move(args));
    });
    ws->SetOnClose([id, weakInstance](IWebSocketResource::CloseCode code, const string &reason) {
      auto strongInstance = weakInstance.lock();
      if (!strongInstance)
        return;

      auto args = dynamic::object("id", id)("code", static_cast<uint16_t>(code))("reason", reason);
      SendEvent(weakInstance, "websocketClosed", std::move(args));
    });

    state->ResourceMap.emplace(id, ws);
    return ws;
  }

  return itr->second;
}

} // anonymous namespace

namespace Microsoft::React {

WebSocketModule::WebSocketModule() : m_sharedState{std::make_shared<SharedState>()} {
  m_sharedState->ResourceFactory = [](string &&url) { return IWebSocketResource::Make(); };
  m_sharedState->Module = this;
}

WebSocketModule::~WebSocketModule() noexcept /*override*/ {
  m_sharedState->Module = nullptr;
}

void WebSocketModule::SetResourceFactory(
    std::function<shared_ptr<IWebSocketResource>(const string &)> &&resourceFactory) {
  m_sharedState->ResourceFactory = std::move(resourceFactory);
}

string WebSocketModule::getName() {
  return moduleName;
}

std::map<string, dynamic> WebSocketModule::getConstants() {
  return {};
}

// clang-format off
std::vector<facebook::xplat::module::CxxModule::Method> WebSocketModule::getMethods()
{
  return
  {
    {
      "connect",
      [weakState = weak_ptr<SharedState>(m_sharedState)](dynamic args) // const string& url, dynamic protocols, dynamic options, int64_t id
      {
        IWebSocketResource::Protocols protocols;
        dynamic protocolsDynamic = jsArgAsDynamic(args, 1);
        if (!protocolsDynamic.empty())
        {
          for (const auto& protocol : protocolsDynamic)
          {
            protocols.push_back(protocol.getString());
          }
        }

        IWebSocketResource::Options options;
        dynamic optionsDynamic = jsArgAsDynamic(args, 2);
        if (!optionsDynamic.empty() && optionsDynamic.count("headers") != 0)
        {
          const auto& headersDynamic = optionsDynamic["headers"];
          for (const auto& header : headersDynamic.items())
          {
            options.emplace(Utf8ToUtf16(header.first.getString()), header.second.getString());
          }
        }

        weak_ptr weakWs = GetOrCreateWebSocket(jsArgAsInt(args, 3), jsArgAsString(args, 0), weakState);
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->Connect(jsArgAsString(args, 0), protocols, options);
        }
      }
    },
    {
      "close",
      [weakState = weak_ptr<SharedState>(m_sharedState)](dynamic args) // [int64_t code, string reason,] int64_t id
      {
        // See react-native\Libraries\WebSocket\WebSocket.js:_close
        if (args.size() == 3) // WebSocketModule.close(statusCode, closeReason, this._socketId);
        {
          weak_ptr weakWs = GetOrCreateWebSocket(jsArgAsInt(args, 2), {}, weakState);
          if (auto sharedWs = weakWs.lock())
          {
            sharedWs->Close(static_cast<IWebSocketResource::CloseCode>(jsArgAsInt(args, 0)), jsArgAsString(args, 1));
          }
        }
        else if (args.size() == 1) // WebSocketModule.close(this._socketId);
        {
          weak_ptr weakWs = GetOrCreateWebSocket(jsArgAsInt(args, 0), {}, weakState);
          if (auto sharedWs = weakWs.lock())
          {
            sharedWs->Close(IWebSocketResource::CloseCode::Normal, {});
          }
        }
        else
        {
          auto state = weakState.lock();
          if (state && state->Module) {
            auto errorObj = dynamic::object("id", -1)("message", "Incorrect number of parameters");
            SendEvent(state->Module->getInstance(), "websocketFailed", std::move(errorObj));
          }
        }
      }
    },
    {
      "send",
      [weakState = weak_ptr<SharedState>(m_sharedState)](dynamic args) // const string& message, int64_t id
      {
        weak_ptr weakWs = GetOrCreateWebSocket(jsArgAsInt(args, 1), {}, weakState);
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->Send(jsArgAsString(args, 0));
        }
      }
    },
    {
      "sendBinary",
      [weakState = weak_ptr<SharedState>(m_sharedState)](dynamic args) // const string& base64String, int64_t id
      {
        weak_ptr weakWs = GetOrCreateWebSocket(jsArgAsInt(args, 1), {}, weakState);
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->SendBinary(jsArgAsString(args, 0));
        }
      }
    },
    {
      "ping",
      [weakState = weak_ptr<SharedState>(m_sharedState)](dynamic args) // int64_t id
      {
        weak_ptr weakWs = GetOrCreateWebSocket(jsArgAsInt(args, 0), {}, weakState);
        if (auto sharedWs = weakWs.lock())
        {
          sharedWs->Ping();
        }
      }
    }
  };
} // getMethods
// clang-format on

/*extern*/ const char *GetWebSocketModuleName() noexcept {
  return moduleName;
}

/*extern*/ std::unique_ptr<facebook::xplat::module::CxxModule> CreateWebSocketModule() noexcept {
  return std::make_unique<WebSocketModule>();
}

} // namespace Microsoft::React
