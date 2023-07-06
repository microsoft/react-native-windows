// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "DefaultBlobResource.h"

#include <Modules/IHttpModuleProxy.h>
#include <Modules/IWebSocketModuleProxy.h>

// Boost Libraries
#include <boost/uuid/uuid_io.hpp>

// Windows API
#include <winrt/Windows.Security.Cryptography.h>

// Standard Library
#include <mutex>

using folly::dynamic;
using std::scoped_lock;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;
using winrt::array_view;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;

namespace msrn = winrt::Microsoft::ReactNative;

namespace {

constexpr char blobKey[] = "blob";
constexpr char blobIdKey[] = "blobId";
constexpr char offsetKey[] = "offset";
constexpr char sizeKey[] = "size";
constexpr char typeKey[] = "type";
constexpr char dataKey[] = "data";

} // namespace

namespace Microsoft::React::Networking {

#pragma region DefaultBlobResource

DefaultBlobResource::DefaultBlobResource(
    shared_ptr<MemoryBlobPersistor> blobPersistor,
    shared_ptr<BlobWebSocketModuleContentHandler> contentHandler,
    shared_ptr<BlobModuleRequestBodyHandler> requestBodyHandler,
    shared_ptr<BlobModuleResponseHandler> responseHandler,
    msrn::ReactPropertyBag propertyBag)
    : m_blobPersistor{blobPersistor},
      m_contentHandler{contentHandler},
      m_requestBodyHandler{requestBodyHandler},
      m_responseHandler{responseHandler},
      m_propertyBag{propertyBag} {}

#pragma region IBlobResource

/*static*/ shared_ptr<IBlobResource> IBlobResource::Make(
    winrt::Windows::Foundation::IInspectable const &inspectableProperties) {
  using namespace msrn;

  auto propBag = ReactPropertyBag{inspectableProperties.try_as<IReactPropertyBag>()};

  auto blobPersistor = std::make_shared<MemoryBlobPersistor>();
  auto contentHandler = std::make_shared<BlobWebSocketModuleContentHandler>(blobPersistor);
  auto requestBodyHanlder = std::make_shared<BlobModuleRequestBodyHandler>(blobPersistor);
  auto responseHandler = std::make_shared<BlobModuleResponseHandler>(blobPersistor);

  auto contentHandlerPropId =
      ReactPropertyId<ReactNonAbiValue<weak_ptr<IWebSocketModuleContentHandler>>>{L"BlobModule.ContentHandler"};
  propBag.Set(contentHandlerPropId, weak_ptr<IWebSocketModuleContentHandler>{contentHandler});

  auto blobPersistorPropId = ReactPropertyId<ReactNonAbiValue<weak_ptr<IBlobPersistor>>>{L"Blob.Persistor"};
  ;
  propBag.Set(blobPersistorPropId, weak_ptr<IBlobPersistor>{blobPersistor});

  auto result = std::make_shared<DefaultBlobResource>(
      blobPersistor, contentHandler, requestBodyHanlder, responseHandler, propBag);

  return result;
}

void DefaultBlobResource::SendOverSocket(string &&blobId, int64_t offset, int64_t size, int64_t socketId) noexcept
/*override*/ {
  auto propId =
      msrn::ReactPropertyId<msrn::ReactNonAbiValue<weak_ptr<IWebSocketModuleProxy>>>{L"WebSocketModule.Proxy"};
  shared_ptr<IWebSocketModuleProxy> wsProxy;
  if (auto prop = m_propertyBag.Get(propId)) {
    wsProxy = prop.Value().lock();
  }
  if (!wsProxy) {
    return;
  }

  array_view<uint8_t const> data;
  try {
    data = m_blobPersistor->ResolveMessage(std::move(blobId), offset, size);
  } catch (const std::exception &e) {
    return m_callbacks.OnError(e.what());
  }

  auto buffer = CryptographicBuffer::CreateFromByteArray(data);
  auto base64Hstring = CryptographicBuffer::EncodeToBase64String(std::move(buffer));
  auto base64String = winrt::to_string(base64Hstring);

  wsProxy->SendBinary(std::move(base64String), socketId);
}

void DefaultBlobResource::CreateFromParts(msrn::JSValueArray &&parts, string &&blobId) noexcept /*override*/ {
  vector<uint8_t> buffer{};

  for (const auto &partItem : parts) {
    auto &part = partItem.AsObject();
    auto type = part.at(typeKey).AsString();
    if (blobKey == type) {
      auto &blob = part.at(dataKey).AsObject();
      array_view<uint8_t const> bufferPart;
      try {
        bufferPart = m_blobPersistor->ResolveMessage(
            blob.at(blobIdKey).AsString(), blob.at(offsetKey).AsInt64(), blob.at(sizeKey).AsInt64());
      } catch (const std::exception &e) {
        return m_callbacks.OnError(e.what());
      }

      buffer.reserve(buffer.size() + bufferPart.size());
      buffer.insert(buffer.end(), bufferPart.begin(), bufferPart.end());
    } else if ("string" == type) {
      auto data = part.at(dataKey).AsString();

      buffer.reserve(buffer.size() + data.size());
      buffer.insert(buffer.end(), data.begin(), data.end());
    } else {
      return m_callbacks.OnError("Invalid type for blob: " + type);
    }
  }

  m_blobPersistor->StoreMessage(std::move(buffer), std::move(blobId));
}

void DefaultBlobResource::Release(string &&blobId) noexcept /*override*/ {
  m_blobPersistor->RemoveMessage(std::move(blobId));
}

void DefaultBlobResource::AddNetworkingHandler() noexcept /*override*/ {
  auto propId = msrn::ReactPropertyId<msrn::ReactNonAbiValue<weak_ptr<IHttpModuleProxy>>>{L"HttpModule.Proxy"};

  if (auto prop = m_propertyBag.Get(propId)) {
    if (auto httpHandler = prop.Value().lock()) {
      httpHandler->AddRequestBodyHandler(m_requestBodyHandler);
      httpHandler->AddResponseHandler(m_responseHandler);
    }
  }
  // TODO: else emit error?
}

void DefaultBlobResource::AddWebSocketHandler(int64_t id) noexcept /*override*/ {
  m_contentHandler->Register(id);
}

void DefaultBlobResource::RemoveWebSocketHandler(int64_t id) noexcept /*override*/ {
  m_contentHandler->Unregister(id);
}

IBlobResource::BlobCallbacks &DefaultBlobResource::Callbacks() noexcept /*override*/ {
  return m_callbacks;
}

#pragma endregion IBlobResource

#pragma endregion DefaultBlobResource

#pragma region MemoryBlobPersistor

#pragma region IBlobPersistor

array_view<uint8_t const> MemoryBlobPersistor::ResolveMessage(string &&blobId, int64_t offset, int64_t size) {
  if (size < 1)
    return {};

  scoped_lock lock{m_mutex};

  auto dataItr = m_blobs.find(std::move(blobId));
  // Not found.
  if (dataItr == m_blobs.cend())
    throw std::invalid_argument("Blob object not found");

  auto &bytes = (*dataItr).second;
  auto endBound = static_cast<size_t>(offset + size);
  // Out of bounds.
  if (endBound > bytes.size() || offset >= static_cast<int64_t>(bytes.size()) || offset < 0)
    throw std::out_of_range("Offset or size out of range");

  return array_view<uint8_t const>(bytes.data() + offset, bytes.data() + endBound);
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
  auto blobId = boost::uuids::to_string(m_guidGenerator());

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

void BlobWebSocketModuleContentHandler::ProcessMessage(
    string &&message,
    msrn::JSValueObject &params) noexcept /*override*/
{
  params[dataKey] = std::move(message);
}

void BlobWebSocketModuleContentHandler::ProcessMessage(
    vector<uint8_t> &&message,
    msrn::JSValueObject &params) noexcept /*override*/
{
  auto blob = msrn::JSValueObject{
      {offsetKey, 0}, {sizeKey, message.size()}, {blobIdKey, m_blobPersistor->StoreMessage(std::move(message))}};

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

bool BlobModuleRequestBodyHandler::Supports(msrn::JSValueObject &data) /*override*/ {
  auto itr = data.find(blobKey);

  return itr != data.cend() && !(*itr).second.AsString().empty();
}

msrn::JSValueObject BlobModuleRequestBodyHandler::ToRequestBody(
    msrn::JSValueObject &data,
    string &contentType) /*override*/ {
  auto type = contentType;
  auto itr = data.find(typeKey);
  if (itr != data.cend() && !(*itr).second.AsString().empty()) {
    type = (*itr).second.AsString();
  }
  if (type.empty()) {
    type = "application/octet-stream";
  }

  auto &blob = data[blobKey].AsObject();
  auto blobId = blob[blobIdKey].AsString();
  auto bytes = m_blobPersistor->ResolveMessage(std::move(blobId), blob[offsetKey].AsInt64(), blob[sizeKey].AsInt64());

  return {{typeKey, type}, {sizeKey, bytes.size()}, {"bytes", msrn::JSValueArray(bytes.cbegin(), bytes.cend())}};
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

msrn::JSValueObject BlobModuleResponseHandler::ToResponseData(vector<uint8_t> &&content) /*override*/ {
  return {{offsetKey, 0}, {sizeKey, content.size()}, {blobIdKey, m_blobPersistor->StoreMessage(std::move(content))}};
}

#pragma endregion IResponseHandler

#pragma endregion BlobModuleResponseHandler

} // namespace Microsoft::React::Networking
