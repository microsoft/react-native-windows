// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "NetworkingModule.h"

#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

using folly::dynamic;
using std::map;
using std::move;
using std::string;
using std::unique_ptr;
using std::vector;

namespace Microsoft::React {

#pragma region NetworkingModule static members

const char *NetworkingModule::name = "Networking";

int64_t NetworkingModule::s_lastRequestId = 0;

#pragma endregion

NetworkingModule::NetworkingModule() {}

IHttpResource *NetworkingModule::GetResource(int64_t requestId) noexcept {
  IHttpResource *ptr = nullptr;
  if (m_resources.find(requestId) == std::end(m_resources)) {
    auto rc = IHttpResource::Make();
    rc->SetOnError([this, requestId](const string &message) {
      // ISS:2306365 - Deal with timeout conditions.
      OnRequestError(requestId, move(message), false /*isTimeOut*/);
    });
    rc->SetOnRequest([this, requestId]() { OnRequestSuccess(requestId); });
    rc->SetOnResponse([this, requestId](const string &message) {
      OnDataReceived(requestId, message);
    });

    ptr = rc.get();
    m_resources.emplace(requestId, move(rc));
  } else {
    ptr = m_resources.at(requestId).get();
  }

  return ptr;
}

void NetworkingModule::OnDataReceived(
    int64_t requestId,
    const string &data) noexcept {
  SendEvent("didReceiveNetworkData", dynamic::array(requestId, data));
}

void NetworkingModule::OnRequestError(
    int64_t requestId,
    const string &error,
    bool isTimeOut) noexcept {
  SendEvent(
      "didCompleteNetworkResponse",
      dynamic::array(requestId, error, isTimeOut));
}

void NetworkingModule::OnRequestSuccess(int64_t requestId) noexcept {
  SendEvent("didCompleteNetworkResponse", dynamic::array(requestId));
}

void NetworkingModule::OnResponseReceived(
    int64_t requestId,
    int64_t statusCode,
    const dynamic &headers,
    const string &url) noexcept {
  SendEvent(
      "didReceiveNetworkResponse",
      dynamic::array(requestId, statusCode, headers, url));
}

void NetworkingModule::SendEvent(string &&eventName, dynamic &&parameters) {
  auto instance = this->getInstance().lock();
  if (instance)
    instance->callJSFunction(
        "RCTDeviceEventEmitter",
        "emit",
        dynamic::array(move(eventName), move(parameters)));
}

#pragma region CxxModule members

string NetworkingModule::getName() {
  return NetworkingModule::name;
}

map<string, dynamic> NetworkingModule::getConstants() {
  return {};
}

vector<facebook::xplat::module::CxxModule::Method>
NetworkingModule::getMethods() {
  return {Method(
              "sendRequest",
              [this](dynamic args, Callback cb) noexcept {
                auto params = facebook::xplat::jsArgAsObject(args, 0);

                auto headers = map<string, string>();
                auto follyHeaders = params["headers"];
                if (!follyHeaders.empty()) {
                  for (auto &header : follyHeaders.items()) {
                    headers[header.first.getString()] =
                        header.second.getString();
                  }
                }

                int64_t requestId = 0; // ISS:2306365 - Manage requestIds.
                GetResource(requestId)->SendRequest(
                    params["method"].asString(),
                    params["url"].asString(),
                    headers,
                    params["data"],
                    params["responseType"].asString(),
                    params["incrementalUpdates"].asBool(),
                    params["timeout"].asInt(),
                    [](int64_t) {} // ISS:2306365 - Convert from 'cb' parameter.
                    );
                // ISS:2306365 - Callback?
              }),
          Method(
              "abortRequest",
              [this](dynamic args) noexcept {
                int64_t requestId = facebook::xplat::jsArgAsInt(args, 0);
                GetResource(requestId)->AbortRequest();
              }),
          Method("clearCookies", [this](dynamic args) noexcept {
            int64_t requestId = facebook::xplat::jsArgAsInt(args, 0);
            GetResource(requestId)->ClearCookies();
          })};
}

#pragma endregion CxxModule members

} // namespace Microsoft::React
