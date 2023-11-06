// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "HttpModule.h"

#include <CreateModules.h>
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
constexpr wchar_t s_moduleNameW[] = L"Networking";

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

msrn::ReactModuleProvider s_moduleProvider = msrn::MakeTurboModuleProvider<Microsoft::React::HttpTurboModule>();

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
  function<void(int64_t, msrn::JSValueObject &&)> onDataDynamic =
      [weakReactInstance](int64_t requestId, msrn::JSValueObject &&responseData) {
        auto responseDynamic = Microsoft::React::Modules::ToDynamic(msrn::JSValue{std::move(responseData)});
        SendEvent(weakReactInstance, receivedData, dynamic::array(requestId, std::move(responseDynamic)));
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

void HttpTurboModule::Initialize(msrn::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
  m_resource = IHttpResource::Make(m_context.Properties().Handle());

  m_resource->SetOnRequestSuccess([context = m_context](int64_t requestId) {
    SendEvent(context, completedResponseW, msrn::JSValueArray{requestId});
  });

  m_resource->SetOnResponse([context = m_context](int64_t requestId, IHttpResource::Response &&response) {
    auto headers = msrn::JSValueObject{};
    for (auto &header : response.Headers) {
      headers[header.first] = header.second;
    }

    // TODO: Test response content?
    auto args = msrn::JSValueArray{requestId, response.StatusCode, std::move(headers), response.Url};

    SendEvent(context, receivedResponseW, std::move(args));
  });

  m_resource->SetOnData([context = m_context](int64_t requestId, string &&responseData) {
    SendEvent(context, receivedDataW, msrn::JSValueArray{requestId, std::move(responseData)});
  });

  // Explicitly declaring function type to avoid type inference ambiguity.
  function<void(int64_t, msrn::JSValueObject &&)> onDataObject =
      [context = m_context](int64_t requestId, msrn::JSValueObject &&responseData) {
        SendEvent(context, receivedDataW, msrn::JSValueArray{requestId, std::move(responseData)});
      };
  m_resource->SetOnData(std::move(onDataObject));

  m_resource->SetOnIncrementalData(
      [context = m_context](int64_t requestId, string &&responseData, int64_t progress, int64_t total) {
        SendEvent(
            context, receivedIncrementalDataW, msrn::JSValueArray{requestId, std::move(responseData), progress, total});
      });

  m_resource->SetOnDataProgress([context = m_context](int64_t requestId, int64_t progress, int64_t total) {
    SendEvent(context, receivedDataProgressW, msrn::JSValueArray{requestId, progress, total});
  });

  m_resource->SetOnResponseComplete([context = m_context](int64_t requestId) {
    SendEvent(context, completedResponseW, msrn::JSValueArray{requestId});
  });

  m_resource->SetOnError([context = m_context](int64_t requestId, string &&message, bool isTimeout) {
    auto args = msrn::JSValueArray{requestId, std::move(message)};
    if (isTimeout) {
      args.push_back(true);
    }

    SendEvent(context, completedResponseW, std::move(args));
  });
}

void HttpTurboModule::SendRequest(
    ReactNativeSpecs::NetworkingIOSSpec_sendRequest_query &&query,
    function<void(double)> const &callback) noexcept {
  m_requestId++;
  auto &headersObj = query.headers.AsObject();
  IHttpResource::Headers headers;
  for (auto &entry : headersObj) {
    headers.emplace(entry.first, entry.second.AsString());
  }

  m_resource->SendRequest(
      std::move(query.method),
      std::move(query.url),
      m_requestId,
      std::move(headers),
      query.data.MoveObject(),
      std::move(query.responseType),
      query.incrementalUpdates,
      static_cast<int64_t>(query.timeout),
      query.withCredentials,
      [&callback](int64_t requestId) { callback({static_cast<double>(requestId)}); });
}

void HttpTurboModule::AbortRequest(double requestId) noexcept {
  m_resource->AbortRequest(static_cast<int64_t>(requestId));
}

void HttpTurboModule::ClearCookies(function<void(bool)> const &callback) noexcept {
  m_resource->ClearCookies();
}

void HttpTurboModule::AddListener(string &&eventName) noexcept { /*NOOP*/
}

void HttpTurboModule::RemoveListeners(double count) noexcept { /*NOOP*/
}

#pragma endregion HttpTurboModule

#pragma region HttpModule

HttpModule::HttpModule(IInspectable const &inspectableProperties) noexcept
    : m_holder{std::make_shared<ModuleHolder>()},
      m_inspectableProperties{inspectableProperties},
      m_resource{IHttpResource::Make(inspectableProperties)} {
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
  // See CxxNativeModule::lazyInit()
  SetUpHttpResource(m_resource, getInstance(), m_inspectableProperties);

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

        holder->Module->m_requestId++;

        auto params = facebook::xplat::jsArgAsObject(args, 0);
        IHttpResource::Headers headers;
        for (auto& header : params["headers"].items()) {
          headers.emplace(header.first.getString(), header.second.getString());
        }

        holder->Module->m_resource->SendRequest(
          params["method"].asString(),
          params["url"].asString(),
          holder->Module->m_requestId,
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

        holder->Module->m_resource->AbortRequest(facebook::xplat::jsArgAsInt(args, 0));
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

        holder->Module->m_resource->ClearCookies();
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

/*extern*/ std::unique_ptr<facebook::xplat::module::CxxModule> CreateHttpModule(
    IInspectable const &inspectableProperties) noexcept {
  return std::make_unique<HttpModule>(inspectableProperties);
}

/*extern*/ const wchar_t *GetHttpTurboModuleName() noexcept {
  return s_moduleNameW;
}

/*extern*/ const msrn::ReactModuleProvider &GetHttpModuleProvider() noexcept {
  return s_moduleProvider;
}

} // namespace Microsoft::React
