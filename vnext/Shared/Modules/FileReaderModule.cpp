// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "FileReaderModule.h"

#include <CreateModules.h>
#include <ReactPropertyBag.h>

// React Native
#include <cxxreact/JsArgumentHelpers.h>

// Windows API
#include <winrt/Windows.Foundation.h>

using namespace facebook::xplat;

namespace msrn = winrt::Microsoft::ReactNative;

using folly::dynamic;
using std::string;
using std::weak_ptr;
using winrt::Windows::Foundation::IInspectable;

namespace {
constexpr char s_moduleName[] = "FileReaderModule";
constexpr wchar_t s_moduleNameW[] = L"FileReaderModule";

msrn::ReactModuleProvider s_moduleProvider = msrn::MakeTurboModuleProvider<Microsoft::React::FileReaderTurboModule>();
} // namespace

namespace Microsoft::React {

#pragma region FileReaderModule

FileReaderModule::FileReaderModule(weak_ptr<IBlobPersistor> weakBlobPersistor) noexcept
    : m_resource{IFileReaderResource::Make(weakBlobPersistor)} {}

FileReaderModule::~FileReaderModule() noexcept /*override*/
{}

#pragma region CxxModule

string FileReaderModule::getName() {
  return s_moduleName;
}

std::map<string, dynamic> FileReaderModule::getConstants() {
  return {};
}

std::vector<module::CxxModule::Method> FileReaderModule::getMethods() {
  return {
      {///
       /// <param name="args">
       /// Array of arguments passed from the JavaScript layer.
       /// [0]  - dynamic blob object { blobId, offset, size[, type] }
       /// </param>
       ///
       "readAsDataURL",
       [resource = m_resource](dynamic args, Callback resolve, Callback reject) {
         auto blob = jsArgAsObject(args, 0);

         auto blobId = blob["blobId"].asString();
         auto offset = blob["offset"].asInt();
         auto size = blob["size"].asInt();

         auto typeItr = blob.find("type");
         string type{};
         if (typeItr == blob.items().end()) {
           type = "application/octet-stream";
         } else {
           type = (*typeItr).second.asString();
         }

         resource->ReadAsDataUrl(
             std::move(blobId),
             offset,
             size,
             std::move(type),
             [&resolve](string &&message) { resolve({std::move(message)}); },
             [&reject](string &&message) { reject({std::move(message)}); });
       }},
      {///
       /// <param name="args">
       /// Array of arguments passed from the JavaScript layer.
       /// [0]  - dynamic blob object { blobId, offset, size }
       /// [1]  - string encoding
       /// </param>
       ///
       "readAsText",
       [resource = m_resource](dynamic args, Callback resolve, Callback reject) {
         auto blob = jsArgAsObject(args, 0);
         auto encoding = jsArgAsString(args, 1); // Default: "UTF-8"

         auto blobId = blob["blobId"].asString();
         auto offset = blob["offset"].asInt();
         auto size = blob["size"].asInt();

         resource->ReadAsText(
             std::move(blobId),
             offset,
             size,
             std::move(encoding),
             [&resolve](string &&message) { resolve({std::move(message)}); },
             [&reject](string &&message) { reject({std::move(message)}); });
       }}};
}

#pragma endregion CxxModule

#pragma endregion FileReaderModule

#pragma region FileReaderTurboModule

void FileReaderTurboModule::Initialize(msrn::ReactContext const &reactContext) noexcept {
  auto propId = msrn::ReactPropertyId<msrn::ReactNonAbiValue<weak_ptr<IBlobPersistor>>>{L"Blob.Persistor"};
  auto props = reactContext.Properties();
  auto prop = props.Get(propId);
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

/*extern*/ const char *GetFileReaderModuleName() noexcept {
  return s_moduleName;
}

/*extern*/ std::unique_ptr<module::CxxModule> CreateFileReaderModule(
    IInspectable const &inspectableProperties) noexcept {
  auto propId = msrn::ReactPropertyId<msrn::ReactNonAbiValue<weak_ptr<IBlobPersistor>>>{L"Blob.Persistor"};
  auto propBag = msrn::ReactPropertyBag{inspectableProperties.try_as<msrn::IReactPropertyBag>()};

  if (auto prop = propBag.Get(propId)) {
    auto weakBlobPersistor = prop.Value();

    return std::make_unique<FileReaderModule>(weakBlobPersistor);
  }

  return nullptr;
}

/*extern*/ const wchar_t *GetFileReaderTurboModuleName() noexcept {
  return s_moduleNameW;
}

/*extern*/ const msrn::ReactModuleProvider &GetFileReaderModuleProvider() noexcept {
  return s_moduleProvider;
}

} // namespace Microsoft::React
