// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "HttpModule.h"

// React Native
#include <cxxreact/Instance.h>

using facebook::react::Instance;
using folly::dynamic;
using std::shared_ptr;
using std::string;
using std::weak_ptr;

namespace {
constexpr char moduleName[] = "Networking";

//TODO: Add to shared header? (See WebSocketModule)
static void SendEvent(weak_ptr<Instance> weakInstance, string &&eventName, dynamic &&args) {
  if (auto instance = weakInstance.lock()) {
    instance->callJSFunction("RCTDeviceEventEmitter", "emit", dynamic::array(std::move(eventName), std::move(args)));
  }
}

} // namespace <anonymous>

namespace Microsoft::React {

HttpModule::HttpModule() noexcept : m_resource{IHttpResource::Make()}, m_holder{std::make_shared<ModuleHolder>()} {
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
  return
  {
    {
      "sendRequest",
      [](dynamic args, Callback cb)
      {
      }
    },
    {
      "abortRequest",
      [](dynamic args)
      {
      }
    },
    {
      "clearCookies",
      [](dynamic args)
      {
      }
    }
  };
}
// clang-format on

#pragma endregion CxxModule
} // namespace Microsoft::React
