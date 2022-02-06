// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "HttpModule.h"

// React Native
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

using facebook::react::Instance;
using folly::dynamic;
using std::shared_ptr;
using std::string;
using std::weak_ptr;

namespace {

using Microsoft::React::IHttpResource;

constexpr char moduleName[] = "Networking";

//TODO: Add to shared header? (See WebSocketModule)
static void SendEvent(weak_ptr<Instance> weakInstance, string &&eventName, dynamic &&args) {
  if (auto instance = weakInstance.lock()) {
    instance->callJSFunction("RCTDeviceEventEmitter", "emit", dynamic::array(std::move(eventName), std::move(args)));
  }
}

static shared_ptr<IHttpResource> CreateHttpResource(weak_ptr<Instance> weakInstance) {
  auto resource = IHttpResource::Make();

  resource->SetOnResponse([weakInstance](int64_t requestId, IHttpResource::Response &&response) {
    dynamic headers = dynamic::object();
    for (auto &header : response.Headers) {
      headers[header.first] = header.second;
    }

    // TODO: Test response content.
    dynamic args = dynamic::array(
      requestId,
      response.StatusCode,
      headers,
      response.Url
    );

    SendEvent(weakInstance, "didReceiveNetworkResponse", std::move(args));
  });

  resource->SetOnData(
      [weakInstance](int64_t requestId, std::string &&responseData) {
      dynamic args = dynamic::array(requestId, std::move(responseData));

      SendEvent(weakInstance, "didReceiveNetworkData", std::move(args));

      //TODO: Move into separate method IF not executed right after onData()
      SendEvent(weakInstance, "didCompleteNetworkResponse", dynamic::array(requestId));
  });

  resource->SetOnError([weakInstance](int64_t requestId, string &&message) {
    dynamic args = dynamic::array(requestId, std::move(message));
    //TODO: isTimeout errorArgs.push_back(true);

    SendEvent(weakInstance, "didCompleteNetworkResponse", std::move(args));
  });

  return resource;
}

} // namespace <anonymous>

namespace Microsoft::React {

HttpModule::HttpModule() noexcept : m_holder{std::make_shared<ModuleHolder>()} {
  m_holder->Module = this;
}

HttpModule::~HttpModule() noexcept /*override*/ {
  m_holder->Module = nullptr;
}

#pragma region CxxModule

string HttpModule::getName() /*override*/ {
  return moduleName;
}

std::map<string, dynamic> HttpModule::getConstants() {
  return {};
}

// clang-format off
std::vector<facebook::xplat::module::CxxModule::Method> HttpModule::getMethods() {

  auto weakHolder = weak_ptr<ModuleHolder>(m_holder);
  auto holder = weakHolder.lock();
  auto weakInstance = weak_ptr<Instance>(holder->Module->getInstance());

  return
  {
    {
      "sendRequest",
      [weakHolder = weak_ptr<ModuleHolder>(m_holder)](dynamic args, Callback cb)//TODO: Check whether 'cb' is needed
      {
        auto holder = weakHolder.lock();
        if (!holder) {
          return;
        }

        auto resource = holder->Module->m_resource;
        if (resource || (resource = CreateHttpResource(holder->Module->getInstance())))
        {
          IHttpResource::BodyData bodyData;
          auto params = facebook::xplat::jsArgAsObject(args, 0);
          auto data = params["data"];
          auto stringData = data["string"];
          if (!stringData.empty())
          {
            bodyData = {IHttpResource::BodyData::Type::String, stringData.getString()};
          }
          else
          {
            auto base64Data = data["base64"];
            if (!base64Data.empty())
            {
              bodyData = {IHttpResource::BodyData::Type::Base64, base64Data.getString()};
            }
            else
            {
              auto uriData = data["uri"];
              if (!uriData.empty())
              {
                bodyData = {IHttpResource::BodyData::Type::Uri, uriData.getString()};
              }
            }
          }
          //TODO: Support FORM data

          IHttpResource::Headers headers;
          for (auto& header : params["headers"].items()) {
            headers.emplace(header.first.getString(), header.second.getString());
          }

          resource->SendRequest(
            params["method"].asString(),
            params["url"].asString(),
            std::move(headers),
            std::move(bodyData),
            params["responseType"].asString(),
            params["incrementalUpdates"].asBool(),
            static_cast<int64_t>(params["timeout"].asDouble()),
            false,//withCredentials,
            {}// callback
          );
        } // If resource available
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
        if (resource || (resource = CreateHttpResource(holder->Module->getInstance())))
        {
          resource->AbortRequest(facebook::xplat::jsArgAsInt(args, 0));
        }
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
        if (resource || (resource = CreateHttpResource(holder->Module->getInstance())))
        {
          resource->ClearCookies();
        }
      }
    }
  };
}
// clang-format on

#pragma endregion CxxModule
} // namespace Microsoft::React
