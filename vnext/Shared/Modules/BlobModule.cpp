// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BlobModule.h"

#include <CreateModules.h>
#include <JSI/JsiApiContext.h>
#include <Modules/CxxModuleUtilities.h>
#include "BlobCollector.h"

using Microsoft::React::Networking::IBlobResource;
using std::string;
using std::vector;
using winrt::Windows::Foundation::IInspectable;

namespace msrn = winrt::Microsoft::ReactNative;

namespace {
constexpr wchar_t s_moduleNameW[] = L"BlobModule";

const auto &blobKeys = IBlobResource::FieldNames();

msrn::ReactModuleProvider s_moduleProvider = msrn::MakeTurboModuleProvider<Microsoft::React::BlobTurboModule>();

} // namespace

namespace Microsoft::React {

#pragma region BlobTurboModule

void BlobTurboModule::Initialize(msrn::ReactContext const &reactContext, facebook::jsi::Runtime &runtime) noexcept {
  m_resource = IBlobResource::Make(reactContext.Properties().Handle());
  m_resource->Callbacks().OnError = [&reactContext](string &&errorText) {
    Modules::SendEvent(reactContext, L"blobFailed", {errorText});
  };

  namespace jsi = facebook::jsi;
  runtime.global().setProperty(
      runtime,
      "__blobCollectorProvider",
      jsi::Function::createFromHostFunction(
          runtime,
          jsi::PropNameID::forAscii(runtime, "__blobCollectorProvider"),
          1,
          [resource = m_resource](jsi::Runtime &rt, const jsi::Value &thisVal, const jsi::Value *args, size_t count) {
            auto blobId = args[0].asString(rt).utf8(rt);
            auto collector = std::make_shared<BlobCollector>(blobId, resource);

            return jsi::Object::createFromHostObject(rt, collector);
          }));
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

/*extern*/ const wchar_t *GetBlobTurboModuleName() noexcept {
  return s_moduleNameW;
}

/*extern*/ const msrn::ReactModuleProvider &GetBlobModuleProvider() noexcept {
  return s_moduleProvider;
}

} // namespace Microsoft::React
