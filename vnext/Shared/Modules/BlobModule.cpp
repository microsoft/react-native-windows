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

// Windows API
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Security.Cryptography.h>

// Standard Library
#include <chrono>
#include <filesystem>
#include <fstream>

using namespace facebook::xplat;

using folly::dynamic;
using std::scoped_lock;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;
using winrt::Microsoft::ReactNative::IReactPropertyBag;
using winrt::Microsoft::ReactNative::ReactNonAbiValue;
using winrt::Microsoft::ReactNative::ReactPropertyBag;
using winrt::Microsoft::ReactNative::ReactPropertyId;
using winrt::Windows::Foundation::GuidHelper;
using winrt::Windows::Foundation::IInspectable;
using winrt::Windows::Foundation::Uri;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;

namespace fs = std::filesystem;

namespace {
constexpr char moduleName[] = "BlobModule";
constexpr char blobKey[] = "blob";
constexpr char blobIdKey[] = "blobId";
constexpr char offsetKey[] = "offset";
constexpr char sizeKey[] = "size";
constexpr char typeKey[] = "type";
constexpr char dataKey[] = "data";

// See https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
const static std::unordered_map<string, const char *> extensionToMime = {
    {".aac", "audio/aac"},
    {".abw", "application/x-abiword"},
    {".arc", "application/x-freearc"},
    {".avif", "image/avif"},
    {".avi", "video/x-msvideo"},
    {".azw", "application/vnd.amazon.ebook"},
    {".bin", "application/octet-stream"},
    {".bmp", "image/bmp"},
    {".bz", "application/x-bzip"},
    {".bz2", "application/x-bzip2"},
    {".cda", "application/x-cdf"},
    {".csh", "application/x-csh"},
    {".css", "text/css"},
    {".csv", "text/csv"},
    {".doc", "application/msword"},
    {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {".eot", "application/vnd.ms-fontobject"},
    {".epub", "application/epub+zip"},
    {".gz", "application/gzip"},
    {".gif", "image/gif"},
    {".htm .html", "text/html"},
    {".ico", "image/vnd.microsoft.icon"},
    {".ics", "text/calendar"},
    {".jar", "application/java-archive"},
    {".jpeg .jpg", "image/jpeg"},
    {".js", "text/javascript"},
    {".json", "application/json"},
    {".jsonld", "application/ld+json"},
    {".mid .midi", "audio/midi audio/x-midi"},
    {".mjs", "text/javascript"},
    {".mp3", "audio/mpeg"},
    {".mp4", "video/mp4"},
    {".mpeg", "video/mpeg"},
    {".mpkg", "application/vnd.apple.installer+xml"},
    {".odp", "application/vnd.oasis.opendocument.presentation"},
    {".ods", "application/vnd.oasis.opendocument.spreadsheet"},
    {".odt", "application/vnd.oasis.opendocument.text"},
    {".oga", "audio/ogg"},
    {".ogv", "video/ogg"},
    {".ogx", "application/ogg"},
    {".opus", "audio/opus"},
    {".otf", "font/otf"},
    {".png", "image/png"},
    {".pdf", "application/pdf"},
    {".php", "application/x-httpd-php"},
    {".ppt", "application/vnd.ms-powerpoint"},
    {".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
    {".rar", "application/vnd.rar"},
    {".rtf", "application/rtf"},
    {".sh", "application/x-sh"},
    {".svg", "image/svg+xml"},
    {".swf", "application/x-shockwave-flash"},
    {".tar", "application/x-tar"},
    {".tif .tiff", "image/tiff"},
    {".ts", "video/mp2t"},
    {".ttf", "font/ttf"},
    {".txt", "text/plain"},
    {".vsd", "application/vnd.visio"},
    {".wav", "audio/wav"},
    {".weba", "audio/webm"},
    {".webm", "video/webm"},
    {".webp", "image/webp"},
    {".woff", "font/woff"},
    {".woff2", "font/woff2"},
    {".xhtml", "application/xhtml+xml"},
    {".xls", "application/vnd.ms-excel"},
    {".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {".xml", "application/xml"},
    {".xul", "application/vnd.mozilla.xul+xml"},
    {".zip", "application/zip"},
    {".3gp", "video/3gpp; audio/3gpp if it doesn't contain video"},
    {".3g2", "video/3gpp2; audio/3gpp2 if it doesn't contain video"},
    {".7z", "application/x-7z-compressed"}}; // extensionToMime
} // namespace

namespace Microsoft::React {

#pragma region BlobModule

BlobModule::BlobModule(winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept
    : m_sharedState{std::make_shared<SharedState>()},
      m_blobPersistor{std::make_shared<MemoryBlobPersistor>()},
      m_contentHandler{std::make_shared<BlobWebSocketModuleContentHandler>(m_blobPersistor)},
      m_requestBodyHandler{std::make_shared<BlobModuleRequestBodyHandler>(m_blobPersistor)},
      m_responseHandler{std::make_shared<BlobModuleResponseHandler>(m_blobPersistor)},
      m_inspectableProperties{inspectableProperties} {
  auto propBag = ReactPropertyBag{m_inspectableProperties.try_as<IReactPropertyBag>()};

  auto contentHandlerPropId =
      ReactPropertyId<ReactNonAbiValue<weak_ptr<IWebSocketModuleContentHandler>>>{L"BlobModule.ContentHandler"};
  auto contentHandler = weak_ptr<IWebSocketModuleContentHandler>{m_contentHandler};
  propBag.Set(contentHandlerPropId, std::move(contentHandler));

  auto blobPersistorPropId = ReactPropertyId<ReactNonAbiValue<weak_ptr<IBlobPersistor>>>{L"Blob.Persistor"};
  auto blobPersistor = weak_ptr<IBlobPersistor>{m_blobPersistor};
  propBag.Set(blobPersistorPropId, std::move(blobPersistor));

  m_sharedState->Module = this;
}

BlobModule::~BlobModule() noexcept /*override*/ {
  m_sharedState->Module = nullptr;
}

#pragma region CxxModule

string BlobModule::getName() {
  return moduleName;
}

std::map<string, dynamic> BlobModule::getConstants() {
  return {{"BLOB_URI_SCHEME", blobKey}, {"BLOB_URI_HOST", {}}};
}

vector<module::CxxModule::Method> BlobModule::getMethods() {
  return {
      {"addNetworkingHandler",
       [propBag = ReactPropertyBag{m_inspectableProperties.try_as<IReactPropertyBag>()},
        requestBodyHandler = m_requestBodyHandler,
        responseHandler = m_responseHandler](dynamic args) {
         auto propId = ReactPropertyId<ReactNonAbiValue<weak_ptr<IHttpModuleProxy>>>{L"HttpModule.Proxy"};

         if (auto prop = propBag.Get(propId)) {
           if (auto httpHandler = prop.Value().lock()) {
             httpHandler->AddRequestBodyHandler(requestBodyHandler);
             httpHandler->AddResponseHandler(responseHandler);
           }
         }
         // TODO: else emit error?
       }},

      {"addWebSocketHandler",
       [contentHandler = m_contentHandler](dynamic args) {
         auto id = jsArgAsInt(args, 0);

         contentHandler->Register(id);
       }},

      {"removeWebSocketHandler",
       [contentHandler = m_contentHandler](dynamic args) {
         auto id = jsArgAsInt(args, 0);

         contentHandler->Unregister(id);
       }},

      {"sendOverSocket",
       [persistor = m_blobPersistor,
        propBag = ReactPropertyBag{m_inspectableProperties.try_as<IReactPropertyBag>()}](dynamic args) {
         auto propId = ReactPropertyId<ReactNonAbiValue<weak_ptr<IWebSocketModuleProxy>>>{L"WebSocketModule.Proxy"};
         shared_ptr<IWebSocketModuleProxy> wsProxy;
         if (auto prop = propBag.Get(propId)) {
           wsProxy = prop.Value().lock();
         }
         if (!wsProxy) {
           return;
         }

         auto blob = jsArgAsObject(args, 0);
         auto blobId = blob[blobIdKey].getString();
         auto offset = blob[offsetKey].getInt();
         auto size = blob[sizeKey].getInt();
         auto socketID = jsArgAsInt(args, 1);

         auto data = persistor->ResolveMessage(std::move(blobId), offset, size);

         auto buffer = CryptographicBuffer::CreateFromByteArray(data);
         auto winrtString = CryptographicBuffer::EncodeToBase64String(std::move(buffer));
         auto base64String = Common::Unicode::Utf16ToUtf8(std::move(winrtString));

         wsProxy->SendBinary(std::move(base64String), socketID);
       }},

      {"createFromParts",
       // As of React Native 0.67, instance is set AFTER CxxModule::getMethods() is invoked.
       // Use getInstance() directly once
       // https://github.com/facebook/react-native/commit/1d45b20b6c6ba66df0485cdb9be36463d96cf182 becomes available.
       [persistor = m_blobPersistor, weakState = weak_ptr<SharedState>(m_sharedState)](dynamic args) {
         auto parts = jsArgAsArray(args, 0); // Array<Object>
         auto blobId = jsArgAsString(args, 1);
         vector<uint8_t> buffer{};

         for (const auto &part : parts) {
           auto type = part[typeKey].asString();
           if (blobKey == type) {
             auto blob = part[dataKey];
             auto bufferPart =
                 persistor->ResolveMessage(blob[blobIdKey].asString(), blob[offsetKey].asInt(), blob[sizeKey].asInt());

             buffer.reserve(buffer.size() + bufferPart.size());
             buffer.insert(buffer.end(), bufferPart.begin(), bufferPart.end());
           } else if ("string" == type) {
             auto data = part[dataKey].asString();

             buffer.reserve(buffer.size() + data.size());
             buffer.insert(buffer.end(), data.begin(), data.end());
           } else {
             if (auto state = weakState.lock()) {
               auto message = "Invalid type for blob: " + type;
               Modules::SendEvent(state->Module->getInstance(), "blobFailed", std::move(message));
             }
             return;
           }
         }

         persistor->StoreMessage(std::move(buffer), std::move(blobId));
       }},

      {"release",
       [persistor = m_blobPersistor](dynamic args) // blobId: string
       {
         auto blobId = jsArgAsString(args, 0);

         persistor->RemoveMessage(std::move(blobId));
       }}};
}

#pragma endregion CxxModule

#pragma endregion BlobModule

#pragma region MemoryBlobPersistor

#pragma region IBlobPersistor

winrt::array_view<uint8_t> MemoryBlobPersistor::ResolveMessage(string &&blobId, int64_t offset, int64_t size) noexcept {
  if (offset < 0 || size < 1)
    return {};

  scoped_lock lock{m_mutex};

  auto dataItr = m_blobs.find(std::move(blobId));
  if (dataItr != m_blobs.cend()) {
    auto &bytes = (*dataItr).second;

    auto endBound = static_cast<size_t>(offset + size);
    // Out of bounds.
    if (endBound > bytes.size())
      return {};

    return winrt::array_view<uint8_t>(bytes.data() + offset, bytes.data() + endBound);
  }

  // Not found.
  return {};
}

void MemoryBlobPersistor::RemoveMessage(string &&blobId) noexcept {
  scoped_lock lock{m_mutex};

  m_blobs.erase(std::move(blobId));
}

void MemoryBlobPersistor::StoreMessage(vector<uint8_t> &&message, string &&blobId) noexcept {
  scoped_lock lock{m_mutex};

  m_blobs.insert_or_assign(std::move(blobId), std::move(message));
}

string MemoryBlobPersistor::StoreMessage(vector<uint8_t> &&message) noexcept {
  // substr(1, 36) strips curly braces from a GUID.
  auto blobId = winrt::to_string(winrt::to_hstring(GuidHelper::CreateNewGuid())).substr(1, 36);

  scoped_lock lock{m_mutex};
  m_blobs.insert_or_assign(blobId, std::move(message));

  return blobId;
}

#pragma endregion IBlobPersistor

#pragma endregion MemoryBlobPersistor

#pragma region BlobWebSocketModuleContentHandler

BlobWebSocketModuleContentHandler::BlobWebSocketModuleContentHandler(shared_ptr<IBlobPersistor> blobPersistor) noexcept
    : m_blobPersistor{blobPersistor} {}

#pragma region IWebSocketModuleContentHandler

void BlobWebSocketModuleContentHandler::ProcessMessage(string &&message, dynamic &params) /*override*/ {
  params[dataKey] = std::move(message);
}

void BlobWebSocketModuleContentHandler::ProcessMessage(vector<uint8_t> &&message, dynamic &params) /*override*/ {
  auto blob = dynamic::object();
  blob(offsetKey, 0);
  blob(sizeKey, message.size());
  blob(blobIdKey, m_blobPersistor->StoreMessage(std::move(message)));

  params[dataKey] = std::move(blob);
  params[typeKey] = blobKey;
}
#pragma endregion IWebSocketModuleContentHandler

void BlobWebSocketModuleContentHandler::Register(int64_t socketID) noexcept {
  scoped_lock lock{m_mutex};
  m_socketIds.insert(socketID);
}

void BlobWebSocketModuleContentHandler::Unregister(int64_t socketID) noexcept {
  scoped_lock lock{m_mutex};

  auto itr = m_socketIds.find(socketID);
  if (itr != m_socketIds.end())
    m_socketIds.erase(itr);
}

#pragma endregion BlobWebSocketModuleContentHandler

#pragma region BlobModuleRequestBodyHandler

BlobModuleRequestBodyHandler::BlobModuleRequestBodyHandler(shared_ptr<IBlobPersistor> blobPersistor) noexcept
    : m_blobPersistor{blobPersistor} {}

#pragma region IRequestBodyHandler

bool BlobModuleRequestBodyHandler::Supports(dynamic &data) /*override*/ {
  auto itr = data.find(blobKey);

  return itr != data.items().end() && !(*itr).second.empty();
}

dynamic BlobModuleRequestBodyHandler::ToRequestBody(dynamic &data, string &contentType) /*override*/ {
  auto type = contentType;
  if (!data[typeKey].isNull() && !data[typeKey].asString().empty()) {
    type = data[typeKey].asString();
  }
  if (type.empty()) {
    type = "application/octet-stream";
  }

  auto blob = data[blobKey];
  auto blobId = blob[blobIdKey].asString();
  auto bytes = m_blobPersistor->ResolveMessage(std::move(blobId), blob[offsetKey].asInt(), blob[sizeKey].asInt());

  auto result = dynamic::object();
  result(typeKey, type);
  result(sizeKey, bytes.size());
  result("bytes", dynamic(bytes.cbegin(), bytes.cend()));

  return result;
}

#pragma endregion IRequestBodyHandler

#pragma endregion BlobModuleRequestBodyHandler

#pragma region BlobModuleResponseHandler

BlobModuleResponseHandler::BlobModuleResponseHandler(shared_ptr<IBlobPersistor> blobPersistor) noexcept
    : m_blobPersistor{blobPersistor} {}

#pragma region IResponseHandler

bool BlobModuleResponseHandler::Supports(string &responseType) /*override*/ {
  return blobKey == responseType;
}

dynamic BlobModuleResponseHandler::ToResponseData(vector<uint8_t> &&content) /*override*/ {
  auto blob = dynamic::object();
  blob(offsetKey, 0);
  blob(sizeKey, content.size());
  blob(blobIdKey, m_blobPersistor->StoreMessage(std::move(content)));

  return blob;
}

#pragma endregion IResponseHandler

#pragma endregion BlobModuleResponseHandler

/*extern*/ const char *GetBlobModuleName() noexcept {
  return moduleName;
}

/*extern*/ std::unique_ptr<facebook::xplat::module::CxxModule> CreateBlobModule(
    IInspectable const &inspectableProperties) noexcept {
  if (auto properties = inspectableProperties.try_as<IReactPropertyBag>())
    return std::make_unique<BlobModule>(properties);

  return nullptr;
}

} // namespace Microsoft::React
