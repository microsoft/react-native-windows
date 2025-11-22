// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BlobModule.h"

#include <CreateModules.h>
#include <JSI/JsiApiContext.h>
#include <Modules/CxxModuleUtilities.h>
#include "BlobCollector.h"
#include "InputValidation.h"

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
  m_context = reactContext;
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
  // VALIDATE Blob ID - PATH TRAVERSAL PROTECTION (P0 Critical - CVSS 8.6)
  try {
    auto blobId = blob[blobKeys.BlobId].AsString();
    Microsoft::ReactNative::InputValidation::PathValidator::ValidateBlobId(blobId);

    // VALIDATE Size - DoS PROTECTION
    if (blob.AsObject().count(blobKeys.Size) > 0) {
      int64_t size = blob[blobKeys.Size].AsInt64();
      if (size > 0) {
        Microsoft::ReactNative::InputValidation::SizeValidator::ValidateSize(
            static_cast<size_t>(size),
            Microsoft::ReactNative::InputValidation::SizeValidator::GetMaxBlobSize(),
            "Blob");
      }
    }

    m_resource->SendOverSocket(
        blob[blobKeys.BlobId].AsString(),
        blob[blobKeys.Offset].AsInt64(),
        blob[blobKeys.Size].AsInt64(),
        static_cast<int64_t>(socketID));
  } catch (const std::exception &ex) {
    Modules::SendEvent(m_context, L"blobFailed", {std::string(ex.what())});
  }
}

void BlobTurboModule::CreateFromParts(vector<msrn::JSValue> &&parts, string &&withId) noexcept {
  //  VALIDATE Blob ID - PATH TRAVERSAL PROTECTION (P0 Critical - CVSS 7.5)
  try {
    Microsoft::ReactNative::InputValidation::PathValidator::ValidateBlobId(withId);

    //  VALIDATE Total Size - DoS PROTECTION
    size_t totalSize = 0;
    for (const auto &part : parts) {
      if (part.AsObject().count("data") > 0) {
        size_t partSize = part["data"].AsString().length();
        // Check for overflow before accumulation
        if (totalSize > SIZE_MAX - partSize) {
          throw Microsoft::ReactNative::InputValidation::InvalidSizeException("Blob parts total size overflow");
        }
        totalSize += partSize;
      }
    }
    Microsoft::ReactNative::InputValidation::SizeValidator::ValidateSize(
        totalSize, Microsoft::ReactNative::InputValidation::SizeValidator::GetMaxBlobSize(), "Blob parts total");

    m_resource->CreateFromParts(std::move(parts), std::move(withId));
  } catch (const std::exception &ex) {
    Modules::SendEvent(m_context, L"blobFailed", {std::string(ex.what())});
  }
}

void BlobTurboModule::Release(string &&blobId) noexcept {
  //  VALIDATE Blob ID - PATH TRAVERSAL PROTECTION (P0 Critical - CVSS 5.0)
  try {
    Microsoft::ReactNative::InputValidation::PathValidator::ValidateBlobId(blobId);
    m_resource->Release(std::move(blobId));
  } catch (const std::exception &) {
    // Silently ignore validation errors - release is best-effort and non-critical
  }
}

#pragma endregion BlobTurboModule

/*extern*/ const wchar_t *GetBlobTurboModuleName() noexcept {
  return s_moduleNameW;
}

/*extern*/ const msrn::ReactModuleProvider &GetBlobModuleProvider() noexcept {
  return s_moduleProvider;
}

} // namespace Microsoft::React
