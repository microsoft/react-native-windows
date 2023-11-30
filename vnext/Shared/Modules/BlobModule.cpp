// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BlobModule.h"

#include <CreateModules.h>
#include <JSI/JsiApiContext.h>
#include <Modules/CxxModuleUtilities.h>
#include "BlobCollector.h"

// React Native
#include <cxxreact/JsArgumentHelpers.h>

using namespace facebook::xplat;

using folly::dynamic;
using Microsoft::React::Networking::IBlobResource;
using std::string;
using std::vector;
using winrt::Windows::Foundation::IInspectable;

namespace msrn = winrt::Microsoft::ReactNative;

namespace {
constexpr char s_moduleName[] = "BlobModule";
constexpr wchar_t s_moduleNameW[] = L"BlobModule";

const auto &blobKeys = IBlobResource::FieldNames();

msrn::ReactModuleProvider s_moduleProvider = msrn::MakeTurboModuleProvider<Microsoft::React::BlobTurboModule>();

} // namespace

namespace Microsoft::React {

#pragma region BlobTurboModule

void BlobTurboModule::Initialize(msrn::ReactContext const &reactContext) noexcept {
  m_resource = IBlobResource::Make(reactContext.Properties().Handle());
  m_resource->Callbacks().OnError = [&reactContext](string &&errorText) {
    Modules::SendEvent(reactContext, L"blobFailed", {errorText});
  };

  namespace jsi = facebook::jsi;
  msrn::ExecuteJsi(reactContext, [resource = m_resource](jsi::Runtime &runtime) {
    runtime.global().setProperty(
        runtime,
        "__blobCollectorProvider",
        jsi::Function::createFromHostFunction(
            runtime,
            jsi::PropNameID::forAscii(runtime, "__blobCollectorProvider"),
            1,
            [resource](jsi::Runtime &rt, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
              auto blobId = args[0].asString(rt).utf8(rt);
              auto collector = std::make_shared<BlobCollector>(blobId, resource);

              return jsi::Object::createFromHostObject(rt, collector);
            }));
  });
}

ReactNativeSpecs::BlobModuleSpec_Constants BlobTurboModule::GetConstants() noexcept {
  ReactNativeSpecs::BlobModuleSpec_Constants result;
  result.BLOB_URI_SCHEME = blobKeys.Blob;
  result.BLOB_URI_HOST = {};

  return result;
}

void BlobTurboModule::AddNetworkingHandler() noexcept {
  m_resource->AddNetworkingHandler();
}

void BlobTurboModule::AddWebSocketHandler(double id) noexcept {
  m_resource->AddWebSocketHandler(static_cast<int64_t>(id));
}

void BlobTurboModule::RemoveWebSocketHandler(double id) noexcept {
  m_resource->RemoveWebSocketHandler(static_cast<int64_t>(id));
}

void BlobTurboModule::SendOverSocket(msrn::JSValue &&blob, double socketID) noexcept {
  m_resource->SendOverSocket(
      blob[blobKeys.BlobId].AsString(),
      blob[blobKeys.Offset].AsInt64(),
      blob[blobKeys.Size].AsInt64(),
      static_cast<int64_t>(socketID));
}

void BlobTurboModule::CreateFromParts(vector<msrn::JSValue> &&parts, string &&withId) noexcept {
  m_resource->CreateFromParts(std::move(parts), std::move(withId));
}

void BlobTurboModule::Release(string &&blobId) noexcept {
  m_resource->Release(std::move(blobId));
}

#pragma endregion BlobTurboModule

#pragma region BlobModule

BlobModule::BlobModule(winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept
    : m_resource{IBlobResource::Make(inspectableProperties)} {}

#pragma region CxxModule

string BlobModule::getName() {
  return s_moduleName;
}

std::map<string, dynamic> BlobModule::getConstants() {
  return {{"BLOB_URI_SCHEME", blobKeys.Blob}, {"BLOB_URI_HOST", {}}};
}

vector<module::CxxModule::Method> BlobModule::getMethods() {
  // See CxxNativeModule::lazyInit()
  m_resource->Callbacks().OnError = [weakInstance = getInstance()](string &&errorText) {
    Modules::SendEvent(weakInstance, "blobFailed", std::move(errorText));
  };

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
       [resource = m_resource](dynamic args) {
         auto blob = jsArgAsObject(args, 0);
         auto blobId = blob[blobKeys.BlobId].getString();
         auto offset = blob[blobKeys.Offset].getInt();
         auto size = blob[blobKeys.Size].getInt();
         auto socketId = jsArgAsInt(args, 1);

         resource->SendOverSocket(std::move(blobId), offset, size, socketId);
       }},

      {"createFromParts",
       [resource = m_resource](dynamic args) {
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
  if (auto properties = inspectableProperties.try_as<msrn::IReactPropertyBag>())
    return std::make_unique<BlobModule>(properties);

  return nullptr;
}

/*extern*/ const wchar_t *GetBlobTurboModuleName() noexcept {
  return s_moduleNameW;
}

/*extern*/ const msrn::ReactModuleProvider &GetBlobModuleProvider() noexcept {
  return s_moduleProvider;
}

} // namespace Microsoft::React
