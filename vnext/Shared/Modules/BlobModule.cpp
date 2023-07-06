// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BlobModule.h"

#include <Modules/CxxModuleUtilities.h>
#include <Modules/IHttpModuleProxy.h>
#include <Modules/IWebSocketModuleProxy.h>
#include <ReactPropertyBag.h>
#include <unicode.h>

// React Native
#include <cxxreact/JsArgumentHelpers.h>

// Boost Libraries
#include <boost/uuid/uuid_io.hpp>

// Windows API
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Security.Cryptography.h>

// Standard Library
#include <chrono>
#include <filesystem>
#include <fstream>

using namespace facebook::xplat;

using folly::dynamic;
using Microsoft::React::Networking::IBlobResource;
using std::scoped_lock;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;
using winrt::Microsoft::ReactNative::IReactPropertyBag;
using winrt::Microsoft::ReactNative::ReactNonAbiValue;
using winrt::Microsoft::ReactNative::ReactPropertyBag;
using winrt::Microsoft::ReactNative::ReactPropertyId;
using winrt::Windows::Foundation::IInspectable;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;

namespace fs = std::filesystem;
namespace msrn = winrt::Microsoft::ReactNative;

namespace {
constexpr char s_moduleName[] = "BlobModule";
constexpr char blobKey[] = "blob";
constexpr char blobIdKey[] = "blobId";
constexpr char offsetKey[] = "offset";
constexpr char sizeKey[] = "size";
constexpr char typeKey[] = "type";
constexpr char dataKey[] = "data";
} // namespace

namespace Microsoft::React {

#pragma region BlobTurboModule

void BlobTurboModule::Initialize(msrn::ReactContext const &reactContext) noexcept {}

ReactNativeSpecs::BlobModuleSpec_Constants BlobTurboModule::GetConstants() noexcept {
  ReactNativeSpecs::BlobModuleSpec_Constants result;
  result.BLOB_URI_SCHEME = blobKey;
  result.BLOB_URI_HOST = {};

  return result;
}

void BlobTurboModule::AddNetworkingHandler() noexcept {}

void BlobTurboModule::AddWebSocketHandler(double id) noexcept {}

void BlobTurboModule::RemoveWebSocketHandler(double id) noexcept {}

void BlobTurboModule::SendOverSocket(msrn::JSValue &&blob, double socketID) noexcept {}

void BlobTurboModule::CreateFromParts(vector<msrn::JSValue> const &parts, string &&withId) noexcept {}

void BlobTurboModule::Release(string &&blobId) noexcept {}

#pragma endregion BlobTurboModule

#pragma region BlobModule

BlobModule::BlobModule(winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept
    : m_sharedState{std::make_shared<SharedState>()} {
  m_sharedState->Module = this;
  m_resource = IBlobResource::Make(inspectableProperties);
  m_resource->Callbacks().OnError = [sharedState = m_sharedState](string &&errorText) {
    Modules::SendEvent(sharedState->Module->getInstance(), "blobFailed", std::move(errorText));
  };
}

BlobModule::~BlobModule() noexcept /*override*/ {
  m_sharedState->Module = nullptr;
}

#pragma region CxxModule

string BlobModule::getName() {
  return s_moduleName;
}

std::map<string, dynamic> BlobModule::getConstants() {
  return {{"BLOB_URI_SCHEME", blobKey}, {"BLOB_URI_HOST", {}}};
}

vector<module::CxxModule::Method> BlobModule::getMethods() {
  return {
      {"addNetworkingHandler", [resource = m_resource](dynamic /*args*/) { resource->AddNetworkingHandler(); }},

      {"addWebSocketHandler",
       [resource = m_resource](dynamic args) {
         auto id = jsArgAsInt(args, 0);

         resource->AddWebSocketHandler(id);
       }},

      {"removeWebSocketHandler",
       [resource = m_resource](dynamic args) {
         auto id = jsArgAsInt(args, 0);

         resource->RemoveWebSocketHandler(id);
       }},

      {"sendOverSocket",
       [weakState = weak_ptr<SharedState>(m_sharedState), resource = m_resource](dynamic args) {
         auto blob = jsArgAsObject(args, 0);
         auto blobId = blob[blobIdKey].getString();
         auto offset = blob[offsetKey].getInt();
         auto size = blob[sizeKey].getInt();
         auto socketId = jsArgAsInt(args, 1);

         resource->SendOverSocket(std::move(blobId), offset, size, socketId);
       }},

      {"createFromParts",
       // As of React Native 0.67, instance is set AFTER CxxModule::getMethods() is invoked.
       // Use getInstance() directly once
       // https://github.com/facebook/react-native/commit/1d45b20b6c6ba66df0485cdb9be36463d96cf182 becomes available.
       [weakState = weak_ptr<SharedState>(m_sharedState), resource = m_resource](dynamic args) {
         auto dynamicParts = jsArgAsArray(args, 0); // Array<Object>
         auto parts = Modules::ToJSValue(dynamicParts);
         auto blobId = jsArgAsString(args, 1);

         resource->CreateFromParts(parts.MoveArray(), std::move(blobId));
       }},

      {"release",
       [resource = m_resource](dynamic args) // blobId: string
       {
         auto blobId = jsArgAsString(args, 0);

         resource->Release(std::move(blobId));
       }}};
}

#pragma endregion CxxModule

#pragma endregion BlobModule

/*extern*/ const char *GetBlobModuleName() noexcept {
  return s_moduleName;
}

/*extern*/ std::unique_ptr<facebook::xplat::module::CxxModule> CreateBlobModule(
    IInspectable const &inspectableProperties) noexcept {
  if (auto properties = inspectableProperties.try_as<IReactPropertyBag>())
    return std::make_unique<BlobModule>(properties);

  return nullptr;
}

} // namespace Microsoft::React
