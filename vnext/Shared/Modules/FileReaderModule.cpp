// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "FileReaderModule.h"

#include <ReactPropertyBag.h>
#include <sstream>

// Boost Library
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/transform_width.hpp>

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
constexpr char moduleName0[] = "FileReaderModule";
} // namespace

namespace Microsoft::React {

#pragma region FileReaderModule

FileReaderModule::FileReaderModule(weak_ptr<IBlobPersistor> weakBlobPersistor) noexcept
    : m_weakBlobPersistor{weakBlobPersistor} {}

FileReaderModule::~FileReaderModule() noexcept /*override*/
{}

#pragma region CxxModule

string FileReaderModule::getName() {
  return moduleName0;
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
       [blobPersistor = m_weakBlobPersistor.lock()](dynamic args, Callback resolve, Callback reject) {
         if (!blobPersistor) {
           return reject({"Could not find Blob persistor"});
         }

         auto blob = jsArgAsObject(args, 0);

         auto blobId = blob["blobId"].asString();
         auto offset = blob["offset"].asInt();
         auto size = blob["size"].asInt();

         winrt::array_view<uint8_t const> bytes;
         try {
           bytes = blobPersistor->ResolveMessage(std::move(blobId), offset, size);
         } catch (const std::exception &e) {
           return reject({e.what()});
         }

         auto result = string{"data:"};
         auto typeItr = blob.find("type");
         if (typeItr == blob.items().end()) {
           result += "application/octet-stream";
         } else {
           result += (*typeItr).second.asString();
         }
         result += ";base64,";

         // https://www.boost.org/doc/libs/1_76_0/libs/serialization/doc/dataflow.html
         using namespace boost::archive::iterators;
         typedef base64_from_binary<transform_width<const char *, 6, 8>> encode_base64;
         std::ostringstream oss;
         std::copy(encode_base64(bytes.cbegin()), encode_base64(bytes.cend()), ostream_iterator<char>(oss));
         result += oss.str();

         resolve({std::move(result)});
       }},
      {///
       /// <param name="args">
       /// Array of arguments passed from the JavaScript layer.
       /// [0]  - dynamic blob object { blobId, offset, size }
       /// [1]  - string encoding
       /// </param>
       ///
       "readAsText",
       [blobPersistor = m_weakBlobPersistor.lock()](dynamic args, Callback resolve, Callback reject) {
         if (!blobPersistor) {
           return reject({"Could not find Blob persistor"});
         }

         auto blob = jsArgAsObject(args, 0);
         auto encoding = jsArgAsString(args, 1); // Default: "UTF-8"

         auto blobId = blob["blobId"].asString();
         auto offset = blob["offset"].asInt();
         auto size = blob["size"].asInt();

         winrt::array_view<uint8_t const> bytes;
         try {
           bytes = blobPersistor->ResolveMessage(std::move(blobId), offset, size);
         } catch (const std::exception &e) {
           return reject({e.what()});
         }

         // #9982 - Handle non-UTF8 encodings
         //         See https://docs.oracle.com/en/java/javase/11/docs/api/java.base/java/nio/charset/Charset.html
         auto result = string{bytes.cbegin(), bytes.cend()};

         resolve({std::move(result)});
       }}};
}

#pragma endregion CxxModule

#pragma endregion FileReaderModule

#pragma region FileReaderTurboModule

void FileReaderTurboModule::Initialize(msrn::ReactContext const& reactContext) noexcept {

}

void FileReaderTurboModule::ReadAsDataUrl(
  msrn::JSValue&& data,
  msrn::ReactPromise<string>&& result) noexcept {
  if (!m_blobPersistor)
    return result.Reject(L"Could not find Blob persistor");
}

void FileReaderTurboModule::ReadAsText(
  msrn::JSValue&& data,
  string&& encoding,
  msrn::ReactPromise<string>&& result) noexcept {
  if (!m_blobPersistor)
    return result.Reject(L"Could not find Blob persistor");
}

#pragma endregion FileReaderTurboModule

/*extern*/ const char *GetFileReaderModuleName() noexcept {
  return moduleName0;
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

} // namespace Microsoft::React
