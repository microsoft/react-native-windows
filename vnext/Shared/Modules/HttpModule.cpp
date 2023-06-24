// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "HttpModule.h"

#include <Modules/CxxModuleUtilities.h>
#include <ReactPropertyBag.h>

// React Native
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

using facebook::react::Instance;
using folly::dynamic;
using std::function;
using std::shared_ptr;
using std::string;
using std::weak_ptr;
using winrt::Microsoft::ReactNative::IReactPropertyBag;
using winrt::Microsoft::ReactNative::ReactNonAbiValue;
using winrt::Microsoft::ReactNative::ReactPropertyBag;
using winrt::Microsoft::ReactNative::ReactPropertyId;
using winrt::Windows::Foundation::IInspectable;

namespace msrn = winrt::Microsoft::ReactNative;

namespace {

using Microsoft::React::Modules::SendEvent;
using Microsoft::React::Networking::IHttpResource;

constexpr char s_moduleName[] = "Networking";

// React event names
constexpr char completedResponse[] = "didCompleteNetworkResponse";
constexpr char receivedResponse[] = "didReceiveNetworkResponse";
constexpr char sentData[] = "didSendNetworkData";
constexpr char receivedIncrementalData[] = "didReceiveNetworkIncrementalData";
constexpr char receivedDataProgress[] = "didReceiveNetworkDataProgress";
constexpr char receivedData[] = "didReceiveNetworkData";

constexpr wchar_t completedResponseW[] = L"didCompleteNetworkResponse";
constexpr wchar_t receivedResponseW[] = L"didReceiveNetworkResponse";
constexpr wchar_t sentDataW[] = L"didSendNetworkData";
constexpr wchar_t receivedIncrementalDataW[] = L"didReceiveNetworkIncrementalData";
constexpr wchar_t receivedDataProgressW[] = L"didReceiveNetworkDataProgress";
constexpr wchar_t receivedDataW[] = L"didReceiveNetworkData";

static void SetUpHttpResource(
    shared_ptr<IHttpResource> resource,
    weak_ptr<Instance> weakReactInstance,
    IInspectable &inspectableProperties) {
  resource->SetOnRequestSuccess([weakReactInstance](int64_t requestId) {
    auto args = dynamic::array(requestId);

    SendEvent(weakReactInstance, completedResponse, std::move(args));
  });

  resource->SetOnResponse([weakReactInstance](int64_t requestId, IHttpResource::Response &&response) {
    dynamic headers = dynamic::object();
    for (auto &header : response.Headers) {
      headers[header.first] = header.second;
    }

    // TODO: Test response content.
    dynamic args = dynamic::array(requestId, response.StatusCode, headers, response.Url);

    SendEvent(weakReactInstance, receivedResponse, std::move(args));
  });

  resource->SetOnData([weakReactInstance](int64_t requestId, string &&responseData) {
    SendEvent(weakReactInstance, receivedData, dynamic::array(requestId, std::move(responseData)));
  });

  // Explicitly declaring function type to avoid type inference ambiguity.
  function<void(int64_t, dynamic &&)> onDataDynamic = [weakReactInstance](
                                                               int64_t requestId, dynamic &&responseData) {
    SendEvent(weakReactInstance, receivedData, dynamic::array(requestId, std::move(responseData)));
  };
  resource->SetOnData(std::move(onDataDynamic));

  resource->SetOnIncrementalData(
      [weakReactInstance](int64_t requestId, string &&responseData, int64_t progress, int64_t total) {
        SendEvent(
            weakReactInstance,
            receivedIncrementalData,
            dynamic::array(requestId, std::move(responseData), progress, total));
      });

  resource->SetOnDataProgress([weakReactInstance](int64_t requestId, int64_t progress, int64_t total) {
    SendEvent(weakReactInstance, receivedDataProgress, dynamic::array(requestId, progress, total));
  });

  resource->SetOnResponseComplete([weakReactInstance](int64_t requestId) {
    SendEvent(weakReactInstance, completedResponse, dynamic::array(requestId));
  });

  resource->SetOnError([weakReactInstance](int64_t requestId, string &&message, bool isTimeout) {
    dynamic args = dynamic::array(requestId, std::move(message));
    if (isTimeout) {
      args.push_back(true);
    }

    SendEvent(weakReactInstance, completedResponse, std::move(args));
  });
}

} // namespace

namespace Microsoft::React {

#pragma region HttpTurboModule

void HttpTurboModule::Initialize(msrn::ReactContext const& reactContext) noexcept {
  m_context = reactContext;
  m_resource = IHttpResource::Make(m_context.Properties().Handle());

  m_resource->SetOnRequestSuccess([context = m_context](int64_t requestId) {
    SendEvent(context, completedResponseW, msrn::JSValueArray{requestId});
    //TODO: create constexpr variant of event name
  });

  m_resource->SetOnResponse([context = m_context](int64_t requestId, IHttpResource::Response&& response) {
    auto headers = msrn::JSValueObject{};
    for (auto& header : response.Headers) {
      headers[header.first] = header.second;
    }

    //TODO: Test response content?
    auto args = msrn::JSValueArray{ requestId, response.StatusCode, std::move(headers), response.Url };

    SendEvent(context, receivedResponseW, std::move(args));
  });

  m_resource->SetOnData([context = m_context](int64_t requestId, string&& responseData) {
    SendEvent(context, receivedDataW, msrn::JSValueArray{requestId, std::move(responseData)});
  });

  // Explicitly declaring function type to avoid type inference ambiguity.
  //TODO
}

void HttpTurboModule::SendRequest(ReactNativeSpecs::NetworkingWindowsSpec_sendRequest_query&& query, function<void(double)> const& callback) noexcept {

}

void HttpTurboModule::AbortRequest(double requestId) noexcept {

}

void HttpTurboModule::ClearCookies(function<void(bool)> const& callback) noexcept {

}

void HttpTurboModule::AddListener(string&& eventName) noexcept {

}

void HttpTurboModule::RemoveListeners(double count) noexcept {

}

#pragma endregion HttpTurboModule

#pragma region HttpModule

HttpModule::HttpModule(IInspectable const& inspectableProperties) noexcept
  : m_holder{ std::make_shared<ModuleHolder>() },
  m_inspectableProperties{ inspectableProperties },
  m_resource{ IHttpResource::Make(inspectableProperties) } {
  m_holder->Module = this;
}

HttpModule::~HttpModule() noexcept /*override*/ {
  m_holder->Module = nullptr;
}

#pragma region CxxModule

string HttpModule::getName() /*override*/ {
  return s_moduleName;
}

std::map<string, dynamic> HttpModule::getConstants() {
  return {};
}

// clang-format off
std::vector<facebook::xplat::module::CxxModule::Method> HttpModule::getMethods() {

  return
  {
    {
      "sendRequest",
      [weakHolder = weak_ptr<ModuleHolder>(m_holder)](dynamic args, Callback cxxCallback)
      {
        auto holder = weakHolder.lock();
        if (!holder) {
          return;
        }

        auto resource = holder->Module->m_resource;
        if (!holder->Module->m_isResourceSetup)
        {
          SetUpHttpResource(resource, holder->Module->getInstance(), holder->Module->m_inspectableProperties);
          holder->Module->m_isResourceSetup = true;
        }

        auto params = facebook::xplat::jsArgAsObject(args, 0);
        IHttpResource::Headers headers;
        for (auto& header : params["headers"].items()) {
          headers.emplace(header.first.getString(), header.second.getString());
        }

        resource->SendRequest(
          params["method"].asString(),
          params["url"].asString(),
          params["requestId"].asInt(),
          std::move(headers),
          Modules::ToJSValue(params["data"]).MoveObject(),
          params["responseType"].asString(),
          params["incrementalUpdates"].asBool(),
          static_cast<int64_t>(params["timeout"].asDouble()),
          params["withCredentials"].asBool(),
          [cxxCallback = std::move(cxxCallback)](int64_t requestId) {
            cxxCallback({requestId});
          }
        );
      }
    },
    {
      "abortRequest",
      [weakHolder = weak_ptr<ModuleHolder>(m_holder)](dynamic args)
      {
        auto holder = weakHolder.lock();
        if (!holder)
        {
          return;
        }

        auto resource = holder->Module->m_resource;
        if (!holder->Module->m_isResourceSetup)
        {
          SetUpHttpResource(resource, holder->Module->getInstance(), holder->Module->m_inspectableProperties);
          holder->Module->m_isResourceSetup = true;
        }

        resource->AbortRequest(facebook::xplat::jsArgAsInt(args, 0));
      }
    },
    {
      "clearCookies",
      [weakHolder = weak_ptr<ModuleHolder>(m_holder)](dynamic args)
      {
        auto holder = weakHolder.lock();
        if (!holder)
        {
          return;
        }

        auto resource = holder->Module->m_resource;
        if (!holder->Module->m_isResourceSetup)
        {
          SetUpHttpResource(resource, holder->Module->getInstance(), holder->Module->m_inspectableProperties);
          holder->Module->m_isResourceSetup = true;
        }

        resource->ClearCookies();
      }
    }
  };
}
// clang-format on

#pragma endregion CxxModule

#pragma endregion HttpModule

/*extern*/ const char *GetHttpModuleName() noexcept {
  return s_moduleName;
}

} // namespace Microsoft::React
