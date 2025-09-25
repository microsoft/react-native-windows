// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "FileReaderModule.h"

#include <CreateModules.h>
#include <ReactPropertyBag.h>
#include "Networking/NetworkPropertyIds.h"

// Windows API
#include <winrt/Windows.Foundation.h>

namespace msrn = winrt::Microsoft::ReactNative;

using std::string;
using std::weak_ptr;
using winrt::Windows::Foundation::IInspectable;

namespace {
constexpr wchar_t s_moduleNameW[] = L"FileReaderModule";

msrn::ReactModuleProvider s_moduleProvider = msrn::MakeTurboModuleProvider<Microsoft::React::FileReaderTurboModule>();
} // namespace

namespace Microsoft::React {

#pragma region FileReaderTurboModule

void FileReaderTurboModule::Initialize(msrn::ReactContext const &reactContext) noexcept {
  auto props = reactContext.Properties();
  auto prop = props.Get(BlobModulePersistorPropertyId());
  m_resource = IFileReaderResource::Make(prop.Value());
}

///
/// <param name="data">
/// Blob object with the following fields:
/// - blobId
/// - offset
/// - size
/// - type (optional)
/// </param>
/// <param name="result">
/// Either resolves or rejects the current method with a given text message.
/// </param>
///
void FileReaderTurboModule::ReadAsDataUrl(msrn::JSValue &&data, msrn::ReactPromise<string> &&result) noexcept {
  auto &blob = data.AsObject();
  auto blobId = blob["blobId"].AsString();
  auto offset = blob["offset"].AsInt64();
  auto size = blob["size"].AsInt64();

  auto typeItr = blob.find("type");
  string type{};
  if (typeItr == blob.end()) {
    type = "application/octet-stream";
  } else {
    type = (*typeItr).second.AsString();
  }

  m_resource->ReadAsDataUrl(
      std::move(blobId),
      offset,
      size,
      std::move(type),
      [&result](string &&message) { result.Resolve(std::move(message)); },
      [&result](string &&message) { result.Reject(winrt::to_hstring(std::move(message)).c_str()); });
}

///
/// <param name="data">
/// Blob object with the following fields:
/// - blobId
/// - offset
/// - size
/// - type (optional)
/// </param>
/// <param name="encoding">
/// Text encoding to proces data with.
/// </param>
/// <param name="result">
/// Either resolves or rejects the current method with a given text message.
/// </param>
///
void FileReaderTurboModule::ReadAsText(
    msrn::JSValue &&data,
    string &&encoding,
    msrn::ReactPromise<string> &&result) noexcept {
  auto &blob = data.AsObject();
  auto blobId = blob["blobId"].AsString();
  auto offset = blob["offset"].AsInt64();
  auto size = blob["size"].AsInt64();

  m_resource->ReadAsText(
      std::move(blobId),
      offset,
      size,
      std::move(encoding),
      [&result](string &&message) { result.Resolve(std::move(message)); },
      [&result](string &&message) { result.Reject(winrt::to_hstring(std::move(message)).c_str()); });
}

#pragma endregion FileReaderTurboModule

/*extern*/ const wchar_t *GetFileReaderTurboModuleName() noexcept {
  return s_moduleNameW;
}

/*extern*/ const msrn::ReactModuleProvider &GetFileReaderModuleProvider() noexcept {
  return s_moduleProvider;
}

} // namespace Microsoft::React
