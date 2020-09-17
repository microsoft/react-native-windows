// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BlobModule.h"

#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

using namespace facebook::xplat;

using folly::dynamic;
using std::shared_ptr;
using std::string;

namespace {
constexpr char moduleName[] = "BlobModule";
}

namespace Microsoft::React {

#pragma region CxxModule overrides

string BlobModule::getName()
{
  return moduleName;
}

std::map<string, dynamic> BlobModule::getConstants() {
  return {};
}

// clang-format off
std::vector<module::CxxModule::Method> BlobModule::getMethods() {
  return
  {
    Method(
      "addNetworkingHandler",
      [this](dynamic args)
      {

      }
    ),

    Method(
      "addWebSocketHandler",
      [this](dynamic args)
      {
        auto id = jsArgAsInt(args, 0);
      }
    ),

    Method(
      "removeWebSocketHandler",
      [this](dynamic args)
      {
        auto id = jsArgAsInt(args, 0);
      }
    ),

    Method(
      "sendOverSocket",
      [this](dynamic args)
      {
        auto blob = jsArgAsObject(args, 0);
        auto socketID = jsArgAsInt(args, 1);
      }
    ),

    Method(
      "createFromParts",
      [this](dynamic args)
      {
        auto parts = jsArgAsArray(args, 0);   // Array<Object>
        auto withId = jsArgAsString(args, 1);
      }
    ),

    Method(
      "release",
      [this](dynamic args)  // blobId: string
      {
        auto blobId = jsArgAsString(args, 0);
      }
    )
  };
}
// clang-format on

#pragma endregion CxxModule overrides

/*extern*/ std::unique_ptr<module::CxxModule> CreateBlobModule() noexcept
{
  return std::make_unique<BlobModule>();
}

} // namespace Microsoft::React
