// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BlobModule.h"

// React Native
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>
#include <cxxreact/ModuleRegistry.h>

// Windows API
#include <winrt/Windows.Foundation.h>

using namespace facebook::xplat;

using folly::dynamic;
using std::lock_guard;
using std::mutex;
using std::shared_ptr;
using std::string;
using std::vector;
using winrt::Windows::Foundation::GuidHelper;

namespace {
constexpr char moduleName[] = "BlobModule";
constexpr char blobURIScheme[] = "blob";

//TODO: Check for memory leaks.
shared_ptr<Microsoft::React::IWebSocketModuleContentHandler> wsContentHandler;
} // namespace

namespace Microsoft::React {

BlobModule::BlobModule() noexcept {
  if (!wsContentHandler)
    wsContentHandler = std::make_shared<BlobWebSocketModuleContentHandler>();
}

#pragma region CxxModule overrides

string BlobModule::getName()
{
  return moduleName;
}

std::map<string, dynamic> BlobModule::getConstants() {
  return {{"BLOB_URI_SCHEME", blobURIScheme}, {"BLOB_URI_HOST", {}}};
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

#pragma region IWebSocketModuleContentHandler overrides

void BlobWebSocketModuleContentHandler::ProcessMessage(string&& message, dynamic& params) /*override*/
{
  params["data"] = std::move(message);
}

void BlobWebSocketModuleContentHandler::ProcessMessage(vector<uint8_t> &&message, dynamic &params) /*override*/
{
  auto blob = dynamic::object();
  blob("offset", 0);
  blob("size", message.size());

  // Equivalent to store()
  // substr(1, 36) strips curly braces from a GUID.
  string blobId = winrt::to_string(winrt::to_hstring(GuidHelper::CreateNewGuid())).substr(1, 36);
  {
    lock_guard<mutex> lock{m_blobsMutex};
    m_blobs.insert_or_assign(blobId, std::move(message));
  }

  params["data"] = std::move(blob);
  params["type"] = "blob";
}

#pragma endregion IWebSocketModuleContentHandler overrides

/*static*/ shared_ptr<IWebSocketModuleContentHandler> IWebSocketModuleContentHandler::GetInstance() noexcept {
  return wsContentHandler;
}

/*extern*/ std::unique_ptr<module::CxxModule> CreateBlobModule() noexcept
{
  return std::make_unique<BlobModule>();
}

} // namespace Microsoft::React
