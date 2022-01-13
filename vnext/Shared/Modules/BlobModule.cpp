// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BlobModule.h"

#include <unicode.h>

// React Native
#include <cxxreact/Instance.h>
#include <cxxreact/JsArgumentHelpers.h>
#include <cxxreact/ModuleRegistry.h>

// Windows API
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Security.Cryptography.h>

using namespace facebook::xplat;

using facebook::react::Instance;
using folly::dynamic;
using std::lock_guard;
using std::mutex;
using std::string;
using std::vector;
using std::weak_ptr;
using winrt::Windows::Foundation::GuidHelper;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;

namespace {
constexpr char moduleName[] = "BlobModule";
constexpr char blobURIScheme[] = "blob";

weak_ptr<Microsoft::React::IWebSocketModuleContentHandler> s_contentHandler;
} // namespace

namespace Microsoft::React {

#pragma region BlobModule

BlobModule::BlobModule() noexcept : m_sharedState{std::make_shared<SharedState>()} {
  m_contentHandler = std::static_pointer_cast<BlobWebSocketModuleContentHandler>(s_contentHandler.lock());
  if (!m_contentHandler) {
    m_contentHandler = std::make_shared<BlobWebSocketModuleContentHandler>();
    s_contentHandler = m_contentHandler;
  }

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
       // As of React Native 0.67, instance is set AFTER CxxModule::getMethods() is invoked.
       // Directly use getInstance() once
       // https://github.com/facebook/react-native/commit/1d45b20b6c6ba66df0485cdb9be36463d96cf182 becomes available.
       [contentHandler = m_contentHandler, weakState = weak_ptr<SharedState>(m_sharedState)](dynamic args) {
         auto blob = jsArgAsObject(args, 0);
         auto blobId = blob["blobId"].getString();
         auto offset = blob["offset"].getInt();
         auto size = blob["size"].getInt();
         auto socketID = jsArgAsInt(args, 1);

         auto data = contentHandler->ResolveMessage(std::move(blobId), offset, size);

         if (auto state = weakState.lock()) {
           auto buffer = CryptographicBuffer::CreateFromByteArray(data);
           auto winrtString = CryptographicBuffer::EncodeToBase64String(std::move(buffer));
           auto base64String = Common::Unicode::Utf16ToUtf8(std::move(winrtString));

           auto sendArgs = dynamic::array(std::move(base64String), socketID);
           if (auto instance = state->Module->getInstance().lock()) {
             instance->callJSFunction("WebSocketModule", "sendBinary", std::move(sendArgs));
           }
         }
       }},

      {"createFromParts",
       [contentHandler = m_contentHandler](dynamic args) {
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
             // TODO: Send error message to instance?
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

vector<uint8_t>
BlobWebSocketModuleContentHandler::ResolveMessage(string &&blobId, int64_t offset, int64_t size) noexcept {
  lock_guard<mutex> lock{m_blobsMutex};

  auto data = m_blobs.at(std::move(blobId));
  auto start = data.cbegin() + static_cast<size_t>(offset);
  auto end = start + static_cast<size_t>(size);

  return vector(start, end);
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

/*static*/ weak_ptr<IWebSocketModuleContentHandler> IWebSocketModuleContentHandler::GetInstance() noexcept {
  return s_contentHandler;
}

/*extern*/ std::unique_ptr<facebook::xplat::module::CxxModule> CreateBlobModule() noexcept {
  return std::make_unique<BlobModule>();
}

} // namespace Microsoft::React
