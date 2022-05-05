// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BlobModule.h"

#include <Modules/IHttpModuleProxy.h>
#include <Modules/IWebSocketModuleProxy.h>
#include <ReactPropertyBag.h>
#include <unicode.h>

// React Native
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

// Windows API
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Security.Cryptography.h>

// Standard Library
#include <queue>

using namespace facebook::xplat;

using facebook::react::Instance;
using folly::dynamic;
using std::queue;
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

namespace {
constexpr char moduleName[] = "BlobModule";
constexpr char blobKey[] = "blob";
constexpr char blobIdKey[] = "blobId";
constexpr char offsetKey[] = "offset";
constexpr char sizeKey[] = "size";
constexpr char typeKey[] = "type";
constexpr char dataKey[] = "data";
} // namespace

namespace Microsoft::React {

#pragma region BlobModule

BlobModule::BlobModule(winrt::Windows::Foundation::IInspectable const &inspectableProperties) noexcept
    : m_sharedState{std::make_shared<SharedState>()},
      m_blobPersistor{std::make_shared<MemoryBlobPersistor>()},
      m_contentHandler{std::make_shared<BlobWebSocketModuleContentHandler>(m_blobPersistor)},
      m_uriHandler{std::make_shared<BlobModuleUriHandler>(m_blobPersistor)},
      m_requestBodyHandler{std::make_shared<BlobModuleRequestBodyHandler>(m_blobPersistor)},
      m_responseHandler{std::make_shared<BlobModuleResponseHandler>(m_blobPersistor)},
      m_inspectableProperties{inspectableProperties} {
  auto propId =
      ReactPropertyId<ReactNonAbiValue<weak_ptr<IWebSocketModuleContentHandler>>>{L"BlobModule.ContentHandler"};
  auto propBag = ReactPropertyBag{m_inspectableProperties.try_as<IReactPropertyBag>()};
  auto contentHandler = weak_ptr<IWebSocketModuleContentHandler>{m_contentHandler};
  propBag.Set(propId, std::move(contentHandler));

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
        uriHandler = m_uriHandler,
        requestBodyHandler = m_requestBodyHandler,
        responseHandler = m_responseHandler](dynamic args) {
         auto propId = ReactPropertyId<ReactNonAbiValue<weak_ptr<IHttpModuleProxy>>>{L"HttpModule.Proxy"};

         if (auto httpHandler = propBag.Get<ReactNonAbiValue<weak_ptr<IHttpModuleProxy>>>(propId).Value().lock()) {
           httpHandler->AddUriHandler(uriHandler);
           httpHandler->AddRequestBodyHandler(requestBodyHandler);
           httpHandler->AddResponseHandler(responseHandler);
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
         auto wsProxy = propBag.Get(propId).Value().lock();
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
               if (auto instance = state->Module->getInstance().lock()) {
                 auto message = "Invalid type for blob: " + type;
                 instance->callJSFunction(
                     "RCTDeviceEventEmitter", "emit", dynamic::array("blobFailed", std::move(message)));
               }
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
  scoped_lock lock{m_mutex};

  auto &data = m_blobs.at(std::move(blobId));

  return winrt::array_view<uint8_t>{data};
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

#pragma region BlobModuleUriHandler

BlobModuleUriHandler::BlobModuleUriHandler(shared_ptr<IBlobPersistor> blobPersistor) noexcept
    : m_blobPersistor{blobPersistor} {}

#pragma region IUriHandler

bool BlobModuleUriHandler::Supports(string &uri, string &responseType) /*override*/ {
  auto uriObj = Uri{winrt::to_hstring(uri)};

  return !(L"http" == uriObj.SchemeName() || L"https" == uriObj.SchemeName()) && blobKey == responseType;
}

dynamic BlobModuleUriHandler::Fetch(string &uri) /*override*/ {
  auto data = vector<uint8_t>{}; // getBytesFromUri

  auto blob = dynamic::object();
  blob(offsetKey, 0);
  blob(sizeKey, data.size());
  blob(typeKey, GetMimeTypeFromUri(uri));
  blob(blobIdKey, m_blobPersistor->StoreMessage(std::move(data)));

  // Needed for files
  blob("name", GetNameFromUri(uri));
  blob("lastModified", GetLastModifiedFromUri(uri));

  return blob;
}

#pragma endregion IUriHandler

string BlobModuleUriHandler::GetMimeTypeFromUri(string &uri) noexcept {
  // TODO: content resolver.
  //  See https://developer.android.com/reference/android/content/ContentResolver
  //  See
  //  https://android.googlesource.com/platform/frameworks/base/+/master/core/java/android/content/ContentResolver.java

  return blobIdKey;
}

string BlobModuleUriHandler::GetNameFromUri(string &uri) noexcept {
  auto uriObj = Uri{winrt::to_hstring(uri)};
  auto path = uriObj.Path();
  if (L"file" == uriObj.SchemeName()) {
    return GetLastPathSegment(path);
  }

  // TODO: Lookup "_display_name"

  return GetLastPathSegment(path);
}

string BlobModuleUriHandler::GetLastPathSegment(winrt::hstring &path) noexcept {
  auto start = path.size();
  auto end = start;
  while (end > 0) {
    if (path[end - 1] != '/') {
      start = end - 1;
      break;
    } else {
      end--;
    }
  }

  // No name characters found
  if (start >= end)
    return {};

  while (start > 0 && path[start - 1] != '/') {
    start--;
  }

  return winrt::to_string(path).substr(start, /*count*/ end - start);
}

int64_t BlobModuleUriHandler::GetLastModifiedFromUri(string &uri) noexcept {
  // TODO: Handle StorageFile URIs
  //  https://stackoverflow.com/questions/31860360

  return 0;
}

#pragma endregion BlobModuleUriHandler

#pragma region BlobModuleRequestBodyHandler

BlobModuleRequestBodyHandler::BlobModuleRequestBodyHandler(shared_ptr<IBlobPersistor> blobPersistor) noexcept
    : m_blobPersistor{blobPersistor} {}

#pragma region IRequestBodyHandler

bool BlobModuleRequestBodyHandler::Supports(dynamic &data) /*override*/ {
  return !data.at(blobKey).empty();
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
  result("bytes", dynamic(bytes.cbegin(), bytes.cend())); //TODO: Confirm key for blob payload.

  return result;
}

#pragma endregion IRequestBodyHandler

#pragma endregion BlobModuleRequestBodyHandler

#pragma region BlobModuleResponseHandler

BlobModuleResponseHandler::BlobModuleResponseHandler(shared_ptr<IBlobPersistor> blobPersistor) noexcept
    : m_blobPersistor{blobPersistor} {}

#pragma region IResponseHandler

bool BlobModuleResponseHandler::Supports(std::string &responseType) /*override*/ {
  return blobKey == responseType;
}

dynamic BlobModuleResponseHandler::ToResponseData(dynamic &body) /*override*/ {
  // TODO: get bytes from body
  auto bytes = vector<uint8_t>{};

  auto blob = dynamic::object();
  blob(offsetKey, 0);
  blob(sizeKey, bytes.size());
  blob(blobIdKey, m_blobPersistor->StoreMessage(std::move(bytes)));

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
