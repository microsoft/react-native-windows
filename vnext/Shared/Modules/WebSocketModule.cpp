// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Modules/WebSocketModule.h>

#include <Modules/CxxModuleUtilities.h>
#include <Modules/IWebSocketModuleContentHandler.h>
#include <ReactPropertyBag.h>

// React Native
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

// Windows API
#include <winrt/Windows.Security.Cryptography.h>

// Standard Libriary
#include <iomanip>

using namespace facebook::xplat;

using facebook::react::Instance;
using folly::dynamic;

using std::shared_ptr;
using std::string;
using std::weak_ptr;

using winrt::Microsoft::ReactNative::IReactPropertyBag;
using winrt::Microsoft::ReactNative::ReactNonAbiValue;
using winrt::Microsoft::ReactNative::ReactPropertyBag;
using winrt::Microsoft::ReactNative::ReactPropertyId;

using winrt::Windows::Foundation::IInspectable;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;

namespace {
using Microsoft::React::IWebSocketModuleProxy;
using Microsoft::React::WebSocketModule;
using Microsoft::React::Modules::SendEvent;
using Microsoft::React::Networking::IWebSocketResource;

constexpr char moduleName[] = "WebSocketModule";

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
    ws->SetOnMessage(
        [id, weakInstance, propBag = ReactPropertyBag{state->InspectableProps.try_as<IReactPropertyBag>()}](
            size_t length, const string &message, bool isBinary) {
          auto strongInstance = weakInstance.lock();
          if (!strongInstance)
            return;

          dynamic args = dynamic::object("id", id)("type", isBinary ? "binary" : "text");
          shared_ptr<Microsoft::React::IWebSocketModuleContentHandler> contentHandler;
          auto propId = ReactPropertyId<ReactNonAbiValue<weak_ptr<Microsoft::React::IWebSocketModuleContentHandler>>>{
              L"BlobModule.ContentHandler"};
          if (auto prop = propBag.Get(propId))
            contentHandler = prop.Value().lock();

          if (contentHandler) {
            if (isBinary) {
              auto buffer = CryptographicBuffer::DecodeFromBase64String(winrt::to_hstring(message));
              winrt::com_array<uint8_t> arr;
              CryptographicBuffer::CopyToByteArray(buffer, arr);
              auto data = std::vector<uint8_t>(arr.begin(), arr.end());

              contentHandler->ProcessMessage(std::move(data), args);
            } else {
              contentHandler->ProcessMessage(string{message}, args);
            }
          } else {
            args["data"] = message;
          }

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

#pragma region WebSocketModule

WebSocketModule::WebSocketModule(winrt::Windows::Foundation::IInspectable const &inspectableProperties)
    : m_sharedState{std::make_shared<SharedState>()},
      m_proxy{std::make_shared<WebSocketModuleProxy>(inspectableProperties)} {
  m_sharedState->ResourceFactory = [](string &&url) { return IWebSocketResource::Make(); };
  m_sharedState->Module = this;
  m_sharedState->InspectableProps = inspectableProperties;

  auto propBag = ReactPropertyBag{m_sharedState->InspectableProps.try_as<IReactPropertyBag>()};

  auto proxyPropId = ReactPropertyId<ReactNonAbiValue<weak_ptr<IWebSocketModuleProxy>>>{L"WebSocketModule.Proxy"};
  auto proxy = weak_ptr<IWebSocketModuleProxy>{m_proxy};
  propBag.Set(proxyPropId, std::move(proxy));

  auto statePropId = ReactPropertyId<ReactNonAbiValue<weak_ptr<SharedState>>>{L"WebSocketModule.SharedState"};
  auto state = weak_ptr<SharedState>{m_sharedState};
  propBag.Set(statePropId, std::move(state));
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
            options.emplace(winrt::to_hstring(header.first.getString()), header.second.getString());
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

#pragma endregion WebSocketModule

#pragma region WebSocketModuleProxy

WebSocketModuleProxy::WebSocketModuleProxy(IInspectable const &inspectableProperties) noexcept
    : m_inspectableProps{inspectableProperties} {}

void WebSocketModuleProxy::SendBinary(std::string &&base64String, int64_t id) noexcept /*override*/ {
  auto propBag = ReactPropertyBag{m_inspectableProps.try_as<IReactPropertyBag>()};
  auto sharedPropId =
      ReactPropertyId<ReactNonAbiValue<weak_ptr<WebSocketModule::SharedState>>>{L"WebSocketModule.SharedState"};
  auto state = propBag.Get(sharedPropId).Value();

  weak_ptr weakWs = GetOrCreateWebSocket(id, {}, std::move(state));
  if (auto sharedWs = weakWs.lock()) {
    sharedWs->SendBinary(std::move(base64String));
  }
}

#pragma endregion WebSocketModuleProxy

/*extern*/ const char *GetWebSocketModuleName() noexcept {
  return moduleName;
}

/*extern*/ std::unique_ptr<facebook::xplat::module::CxxModule> CreateWebSocketModule(
    IInspectable const &inspectableProperties) noexcept {
  if (auto properties = inspectableProperties.try_as<IReactPropertyBag>())
    return std::make_unique<WebSocketModule>(properties);

  return nullptr;
}

} // namespace Microsoft::React
