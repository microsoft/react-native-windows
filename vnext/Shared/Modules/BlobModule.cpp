// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BlobModule.h"

#include <Modules/IWebSocketModuleProxy.h>
#include <unicode.h>

// React Native
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>

// Windows API
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Security.Cryptography.h>

using namespace facebook::xplat;

using facebook::react::Instance;
using folly::dynamic;
using std::lock_guard;
using std::mutex;
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
using winrt::Windows::Foundation::GuidHelper;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;

namespace {
constexpr char moduleName[] = "BlobModule";
constexpr char blobURIScheme[] = "blob";
} // namespace

namespace Microsoft::React {

#pragma region BlobModule

BlobModule::BlobModule(winrt::Windows::Foundation::IInspectable const &iProperties) noexcept
    : m_sharedState{std::make_shared<SharedState>()},
      m_contentHandler{std::make_shared<BlobWebSocketModuleContentHandler>()},
      m_iProperties{iProperties} {

  auto propId = ReactPropertyId<ReactNonAbiValue<shared_ptr<IWebSocketModuleContentHandler>>>{L"BlobModule.ContentHandler"};
  auto propBag = ReactPropertyBag{m_iProperties.try_as<IReactPropertyBag>()};
  auto contentHandler = m_contentHandler;
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
  return {{"BLOB_URI_SCHEME", blobURIScheme}, {"BLOB_URI_HOST", {}}};
}

std::vector<module::CxxModule::Method> BlobModule::getMethods() {
  return {
      {"addNetworkingHandler",
       [](dynamic args) {
         // TODO: Implement #6081
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
       [contentHandler = m_contentHandler,
        propBag = ReactPropertyBag{m_iProperties.try_as<IReactPropertyBag>()}](dynamic args) {
         auto propId = ReactPropertyId<ReactNonAbiValue<shared_ptr<IWebSocketModuleProxy>>>{L"WebSocketModule.Proxy"};
         auto wsProxy = propBag.Get(propId).Value();
         if (!wsProxy) {
           return;
         }

         auto blob = jsArgAsObject(args, 0);
         auto blobId = blob["blobId"].getString();
         auto offset = blob["offset"].getInt();
         auto size = blob["size"].getInt();
         auto socketID = jsArgAsInt(args, 1);

         auto data = contentHandler->ResolveMessage(std::move(blobId), offset, size);

         auto buffer = CryptographicBuffer::CreateFromByteArray(data);
         auto winrtString = CryptographicBuffer::EncodeToBase64String(std::move(buffer));
         auto base64String = Common::Unicode::Utf16ToUtf8(std::move(winrtString));

         wsProxy->SendBinary(std::move(base64String), socketID);
       }},

      {"createFromParts",
       // As of React Native 0.67, instance is set AFTER CxxModule::getMethods() is invoked.
       // Directly use getInstance() once
       // https://github.com/facebook/react-native/commit/1d45b20b6c6ba66df0485cdb9be36463d96cf182 becomes available.
       [contentHandler = m_contentHandler, weakState = weak_ptr<SharedState>(m_sharedState)](dynamic args) {
         auto parts = jsArgAsArray(args, 0); // Array<Object>
         auto blobId = jsArgAsString(args, 1);
         vector<uint8_t> buffer{};

         for (const auto &part : parts) {
           auto type = part["type"];
           if (type == "blob") {
             auto blob = part["data"];
             auto bufferPart = contentHandler->ResolveMessage(
                 blob["blobId"].asString(), blob["offset"].asInt(), blob["size"].asInt());
             buffer.reserve(buffer.size() + bufferPart.size());
             buffer.insert(buffer.end(), bufferPart.begin(), bufferPart.end());
           } else if (type == "string") {
             auto data = part["data"].asString();
             auto bufferPart = vector<uint8_t>(data.begin(), data.end());

             buffer.reserve(buffer.size() + bufferPart.size());
             buffer.insert(buffer.end(), bufferPart.begin(), bufferPart.end());
           } else {
             if (auto state = weakState.lock()) {
               if (auto instance = state->Module->getInstance().lock()) {
                 string message = "Invalid type for blob: " + type.asString();
                 instance->callJSFunction(
                     "RCTDeviceEventEmitter", "emit", dynamic::array("blobFailed", std::move(message)));
               }
             }
             return;
           }

           contentHandler->StoreMessage(std::move(buffer), std::move(blobId));
         }
       }},

      {"release",
       [contentHandler = m_contentHandler](dynamic args) // blobId: string
       {
         auto blobId = jsArgAsString(args, 0);

         contentHandler->RemoveMessage(std::move(blobId));
       }}};
}

#pragma endregion CxxModule

#pragma endregion BlobModule

#pragma region BlobWebSocketModuleContentHandler

#pragma region IWebSocketModuleContentHandler

void BlobWebSocketModuleContentHandler::ProcessMessage(string &&message, dynamic &params) /*override*/ {
  params["data"] = std::move(message);
}

void BlobWebSocketModuleContentHandler::ProcessMessage(vector<uint8_t> &&message, dynamic &params) /*override*/ {
  auto blob = dynamic::object();
  blob("offset", 0);
  blob("size", message.size());

  // substr(1, 36) strips curly braces from a GUID.
  string blobId = winrt::to_string(winrt::to_hstring(GuidHelper::CreateNewGuid())).substr(1, 36);
  StoreMessage(std::move(message), std::move(blobId));

  params["data"] = std::move(blob);
  params["type"] = "blob";
}
#pragma endregion IWebSocketModuleContentHandler

void BlobWebSocketModuleContentHandler::Register(int64_t socketID) noexcept {
  lock_guard<mutex> lock{m_socketIDsMutex};
  m_socketIDs.insert(socketID);
}

void BlobWebSocketModuleContentHandler::Unregister(int64_t socketID) noexcept {
  lock_guard<mutex> lock{m_socketIDsMutex};
  if (m_socketIDs.find(socketID) != m_socketIDs.end())
    m_socketIDs.erase(socketID);
}

winrt::array_view<uint8_t>
BlobWebSocketModuleContentHandler::ResolveMessage(string &&blobId, int64_t offset, int64_t size) noexcept {
  lock_guard<mutex> lock{m_blobsMutex};

  auto &data = m_blobs.at(std::move(blobId));

  return winrt::array_view<uint8_t>{data};
}

void BlobWebSocketModuleContentHandler::RemoveMessage(string &&blobId) noexcept {
  lock_guard<mutex> lock{m_blobsMutex};

  m_blobs.erase(std::move(blobId));
}

void BlobWebSocketModuleContentHandler::StoreMessage(vector<uint8_t> &&message, string &&blobId) noexcept {
  lock_guard<mutex> lock{m_blobsMutex};

  m_blobs.insert_or_assign(std::move(blobId), std::move(message));
}

#pragma endregion BlobWebSocketModuleContentHandler

#pragma region WebSocketModuleContentHandler

void WebSocketModuleContentHandler::ProcessMessage(string&& message, dynamic& params)
{
  params["data"] = std::move(message);
}

void WebSocketModuleContentHandler::ProcessMessage(vector<uint8_t>&& message, dynamic& params)
{
  auto blob = dynamic::object();
  blob("offset", 0);
  blob("size", message.size());

  // substr(1, 36) strips curly braces from a GUID
  string blobId = winrt::to_string(winrt::to_hstring(GuidHelper::CreateNewGuid())).substr(1, 36);
  //TODO: StoreMessage

  params["data"] = std::move(blob);
  params["type"] = "blob";
}

void WebSocketModuleContentHandler::Register(int64_t socketId) noexcept
{
  scoped_lock lock{m_mutex};
  m_socketIds.insert(socketId);
}

void WebSocketModuleContentHandler::Unregister(int64_t socketId) noexcept
{
  scoped_lock lock{m_mutex};
  if (m_socketIds.find(socketId) != m_socketIds.end())
    m_socketIds.erase(socketId);
}

winrt::array_view<uint8_t>
WebSocketModuleContentHandler::ResolveMessage(string&& blobId, int64_t offset, int64_t size) noexcept
{
  scoped_lock lock{m_mutex};
  auto &data = m_blobs.at(std::move(blobId));

  return winrt::array_view<uint8_t>{data};
}

void WebSocketModuleContentHandler::StoreMessage(vector<uint8_t>&& message, string&& blobId) noexcept
{
  scoped_lock lock{m_mutex};
  m_blobs.insert_or_assign(std::move(blobId), std::move(message));
}

void WebSocketModuleContentHandler::RemoveMessage(string&& blobId) noexcept
{
  scoped_lock lock{m_mutex};
  m_blobs.erase(std::move(blobId));
}

#pragma endregion WebSocketModuleContentHandler

/*extern*/ const char *GetBlobModuleName() noexcept {
  return moduleName;
}

/*extern*/ std::unique_ptr<facebook::xplat::module::CxxModule> CreateBlobModule(IInspectable const& iProperties) noexcept {
  if (auto properties = iProperties.try_as<IReactPropertyBag>())
    return std::make_unique<BlobModule>(properties);

  return nullptr;
}

} // namespace Microsoft::React
