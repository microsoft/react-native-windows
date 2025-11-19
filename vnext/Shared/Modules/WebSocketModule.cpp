// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Modules/WebSocketModule.h>
#include <Modules/WebSocketTurboModule.h>

#include <CreateModules.h>
#include <Modules/CxxModuleUtilities.h>
#include <Modules/IWebSocketModuleContentHandler.h>
#include <ReactPropertyBag.h>
#include "InputValidation.h"
#include "Networking/NetworkPropertyIds.h"

// fmt
#include <fmt/format.h>

// Windows API
#include <winrt/Windows.Security.Cryptography.h>

// Standard Library
#include <iomanip>

namespace msrn = winrt::Microsoft::ReactNative;
using folly::dynamic;

using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;

using winrt::Microsoft::ReactNative::IReactPropertyBag;
using winrt::Microsoft::ReactNative::ReactNonAbiValue;
using winrt::Microsoft::ReactNative::ReactPropertyBag;
using winrt::Microsoft::ReactNative::ReactPropertyId;

using winrt::Windows::Foundation::IInspectable;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;

namespace {
using Microsoft::React::IWebSocketModuleProxy;
using Microsoft::React::Modules::SendEvent;
using Microsoft::React::Networking::IWebSocketResource;

constexpr wchar_t s_moduleNameW[] = L"WebSocketModule";

msrn::ReactModuleProvider s_moduleProvider = msrn::MakeTurboModuleProvider<Microsoft::React::WebSocketTurboModule>();

} // anonymous namespace

namespace Microsoft::React {

#pragma region WebSocketTurboModule

shared_ptr<IWebSocketResource> WebSocketTurboModule::CreateResource(int64_t id, string &&url) noexcept {
  shared_ptr<IWebSocketResource> rc;
  try {
    rc = IWebSocketResource::Make();
  } catch (const winrt::hresult_error &e) {
    auto error = fmt::format("[0x{:0>8x}] {}", static_cast<uint32_t>(e.code()), winrt::to_string(e.message()));
    SendEvent(m_context, L"webSocketFailed", {{"id", id}, {"message", std::move(error)}});

    return nullptr;
  } catch (const std::exception &e) {
    SendEvent(m_context, L"webSocketFailed", {{"id", id}, {"message", e.what()}});

    return nullptr;
  } catch (...) {
    SendEvent(
        m_context, L"webSocketFailed", {{"id", id}, {"message", "Unidentified error creating IWebSocketResource"}});

    return nullptr;
  }

  // Set up resource
  rc->SetOnConnect([id, context = m_context]() {
    SendEvent(context, L"websocketOpen", msrn::JSValueObject{{"id", id}});
  });

  rc->SetOnMessage([id, context = m_context](size_t length, const string &message, bool isBinary) {
    auto args = msrn::JSValueObject{{"id", id}, {"type", isBinary ? "binary" : "text"}};
    shared_ptr<IWebSocketModuleContentHandler> contentHandler;
    auto propBag = context.Properties();
    if (auto prop = propBag.Get(BlobModuleContentHandlerPropertyId()))
      contentHandler = prop.Value().lock();

    if (contentHandler) {
      if (isBinary) {
        auto buffer = CryptographicBuffer::DecodeFromBase64String(winrt::to_hstring(message));
        winrt::com_array<uint8_t> arr;
        CryptographicBuffer::CopyToByteArray(buffer, arr);
        auto data = vector<uint8_t>(arr.begin(), arr.end());

        contentHandler->ProcessMessage(std::move(data), args);
      } else {
        contentHandler->ProcessMessage(string{message}, args);
      }
    } else {
      args["data"] = message;
    }

    SendEvent(context, L"websocketMessage", std::move(args));
  });

  rc->SetOnClose([id, context = m_context](IWebSocketResource::CloseCode code, const string &reason) {
    auto args = msrn::JSValueObject{{"id", id}, {"code", static_cast<uint16_t>(code)}, {"reason", reason}};

    SendEvent(context, L"websocketClosed", std::move(args));
  });

  rc->SetOnError([id, context = m_context](const IWebSocketResource::Error &err) {
    auto errorObj = msrn::JSValueObject{{"id", id}, {"message", err.Message}};

    SendEvent(context, L"websocketFailed", std::move(errorObj));
  });

  m_resourceMap.emplace(static_cast<double>(id), rc);

  return rc;
}

void WebSocketTurboModule::Initialize(msrn::ReactContext const &reactContext) noexcept {
  m_context = reactContext.Handle();
  m_proxy = std::make_shared<WebSocketTurboModuleProxy>(m_resourceMap);

  auto proxy = weak_ptr<IWebSocketModuleProxy>{m_proxy};
  m_context.Properties().Set(WebSocketModuleProxyPropertyId(), std::move(proxy));
}

void WebSocketTurboModule::Connect(
    string &&url,
    std::optional<vector<string>> protocols,
    ReactNativeSpecs::WebSocketModuleSpec_connect_options &&options,
    double socketID) noexcept {
  //  VALIDATE URL - SSRF PROTECTION (P0 Critical - CVSS 9.0)
  // RNW is a developer platform - allow localhost by default for Metro, tests, and dev scenarios.
#ifdef RNW_STRICT_SDL
  bool allowLocalhost = false; // Strict SDL mode: block localhost for production apps
#else
  bool allowLocalhost = true; // Developer-friendly: allow localhost for Metro, tests, and development
#endif
  try {
    Microsoft::ReactNative::InputValidation::URLValidator::ValidateURL(url, {"ws", "wss"}, allowLocalhost);
  } catch (const Microsoft::ReactNative::InputValidation::ValidationException &ex) {
    SendEvent(m_context, L"websocketFailed", {{"id", static_cast<int64_t>(socketID)}, {"message", ex.what()}});
    return;
  }

  IWebSocketResource::Protocols rcProtocols;
  for (const auto &protocol : protocols.value_or(vector<string>{})) {
    rcProtocols.push_back(protocol);
  }

  IWebSocketResource::Options rcOptions;
  auto &optHeaders = options.headers;
  if (optHeaders.has_value()) {
    auto &headersVal = optHeaders.value();
    for (const auto &entry : headersVal.AsObject()) {
      // Each header JSValueObject should only contain one key-value pair
      rcOptions.emplace(winrt::to_hstring(entry.first), entry.second.AsString());
    }
  }

  weak_ptr<IWebSocketResource> weakRc;
  auto rcItr = m_resourceMap.find(socketID);
  if (rcItr != m_resourceMap.cend()) {
    weakRc = (*rcItr).second;
  } else {
    weakRc = CreateResource(static_cast<int64_t>(socketID), string{url});
  }

  if (auto rc = weakRc.lock()) {
    rc->Connect(std::move(url), rcProtocols, rcOptions);
  }
}

void WebSocketTurboModule::Close(double code, string &&reason, double socketID) noexcept {
  //  VALIDATE Reason Length - WebSocket Spec (P1 - CVSS 5.0)
  try {
    Microsoft::ReactNative::InputValidation::SizeValidator::ValidateSize(
        reason.length(),
        Microsoft::ReactNative::InputValidation::SizeValidator::MAX_CLOSE_REASON,
        "WebSocket close reason");
  } catch (const Microsoft::ReactNative::InputValidation::ValidationException &ex) {
    SendEvent(m_context, L"websocketFailed", {{"id", static_cast<int64_t>(socketID)}, {"message", ex.what()}});
    return;
  }

  auto rcItr = m_resourceMap.find(socketID);
  if (rcItr == m_resourceMap.cend()) {
    return; // TODO: Send error instead?
  }

  weak_ptr<IWebSocketResource> weakRc = (*rcItr).second;
  if (auto rc = weakRc.lock()) {
    rc->Close(static_cast<IWebSocketResource::CloseCode>(code), std::move(reason));
  }
}

void WebSocketTurboModule::Send(string &&message, double forSocketID) noexcept {
  //  VALIDATE Size - DoS PROTECTION (P0 Critical - CVSS 7.0)
  try {
    Microsoft::ReactNative::InputValidation::SizeValidator::ValidateSize(
        message.length(), Microsoft::ReactNative::InputValidation::GetMaxWebSocketFrame(), "WebSocket message");
  } catch (const Microsoft::ReactNative::InputValidation::ValidationException &ex) {
    SendEvent(m_context, L"websocketFailed", {{"id", static_cast<int64_t>(forSocketID)}, {"message", ex.what()}});
    return;
  }

  auto rcItr = m_resourceMap.find(forSocketID);
  if (rcItr == m_resourceMap.cend()) {
    return; // TODO: Send error instead?
  }

  weak_ptr<IWebSocketResource> weakRc = (*rcItr).second;
  if (auto rc = weakRc.lock()) {
    rc->Send(std::move(message));
  }
}

void WebSocketTurboModule::SendBinary(string &&base64String, double forSocketID) noexcept {
  //  VALIDATE Base64 Format - DoS PROTECTION (P0 Critical - CVSS 7.0)
  try {
    if (!Microsoft::ReactNative::InputValidation::EncodingValidator::IsValidBase64(base64String)) {
      throw Microsoft::ReactNative::InputValidation::InvalidEncodingException("Invalid base64 format");
    }

    //  VALIDATE Size - DoS PROTECTION
    size_t estimatedSize =
        Microsoft::ReactNative::InputValidation::EncodingValidator::EstimateBase64DecodedSize(base64String);
    Microsoft::ReactNative::InputValidation::SizeValidator::ValidateSize(
        estimatedSize, Microsoft::ReactNative::InputValidation::GetMaxWebSocketFrame(), "WebSocket binary frame");
  } catch (const std::exception &ex) {
    SendEvent(m_context, L"websocketFailed", {{"id", static_cast<int64_t>(forSocketID)}, {"message", ex.what()}});
    return;
  }

  auto rcItr = m_resourceMap.find(forSocketID);
  if (rcItr == m_resourceMap.cend()) {
    return; // TODO: Send error instead?
  }

  weak_ptr<IWebSocketResource> weakRc = (*rcItr).second;
  if (auto rc = weakRc.lock()) {
    rc->SendBinary(std::move(base64String));
  }
}

void WebSocketTurboModule::Ping(double socketID) noexcept {
  auto rcItr = m_resourceMap.find(socketID);
  if (rcItr == m_resourceMap.cend()) {
    return; // TODO: Send error instead?
  }

  weak_ptr<IWebSocketResource> weakRc = (*rcItr).second;
  if (auto rc = weakRc.lock()) {
    rc->Ping();
  }
}

// See react-native/ReactAndroid/src/main/java/com/facebook/react/modules/websocket/WebSocketModule.java
void WebSocketTurboModule::AddListener(string && /*eventName*/) noexcept {}

void WebSocketTurboModule::RemoveListeners(double /*count*/) noexcept {}

#pragma endregion WebSocketTurboModule

#pragma region WebSocketTurboModuleProxy

WebSocketTurboModuleProxy::WebSocketTurboModuleProxy(
    std::unordered_map<double, shared_ptr<IWebSocketResource>> &resourceMap) noexcept
    : m_resourceMap{resourceMap} {}

#pragma endregion WebSocketTurboModuleProxy

void WebSocketTurboModuleProxy::SendBinary(string &&base64String, int64_t id) noexcept /*override*/
{
  auto rcItr = m_resourceMap.find(static_cast<double>(id));
  if (rcItr == m_resourceMap.cend()) {
    return;
  }

  weak_ptr<IWebSocketResource> weakRc = (*rcItr).second;
  if (auto rc = weakRc.lock()) {
    rc->SendBinary(std::move(base64String));
  }
}

#pragma region WebSocketTurboModule

/*extern*/ const wchar_t *GetWebSocketTurboModuleName() noexcept {
  return s_moduleNameW;
}

/*extern*/ const msrn::ReactModuleProvider &GetWebSocketModuleProvider() noexcept {
  return s_moduleProvider;
}

} // namespace Microsoft::React
