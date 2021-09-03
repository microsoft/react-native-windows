// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "HttpModule.h"

using folly::dynamic;
using std::shared_ptr;
using std::string;
using std::weak_ptr;

namespace {
constexpr char moduleName[] = "Networking";
}

namespace Microsoft::React {

HttpModule::HttpModule() {}

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

}
