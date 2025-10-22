// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "HttpModule.h"
#include "InputValidation.h"

#include <CreateModules.h>
#include <Modules/CxxModuleUtilities.h>
#include <ReactPropertyBag.h>

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
  
  // SDL Compliance: Validate headers for CRLF injection (P2 - CVSS 4.5)
  try {
    for (auto &entry : headersObj) {
      std::string headerValue = entry.second.AsString();
      Microsoft::ReactNative::InputValidation::EncodingValidator::ValidateHeaderValue(headerValue);
      headers.emplace(entry.first, std::move(headerValue));
    }
  } catch (const Microsoft::ReactNative::InputValidation::ValidationException& ex) {
    // Reject the request by not calling callback
    return;
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
  // SDL Compliance: Validate request ID range (P2 - CVSS 3.5)
  try {
    Microsoft::ReactNative::InputValidation::SizeValidator::ValidateInt32Range(
        static_cast<int32_t>(requestId), 
        0, 
        INT32_MAX, 
        "Request ID");
  } catch (const Microsoft::ReactNative::InputValidation::ValidationException&) {
    // Invalid request ID, ignore abort
    return;
  }
  
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

/*extern*/ const wchar_t *GetHttpTurboModuleName() noexcept {
  return s_moduleNameW;
}

/*extern*/ const msrn::ReactModuleProvider &GetHttpModuleProvider() noexcept {
  return s_moduleProvider;
}

} // namespace Microsoft::React
