// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "DefaultBlobResource.h"

#include <Modules/IHttpModuleProxy.h>
#include <Modules/IWebSocketModuleProxy.h>
#include <utilities.h>

// Boost Libraries
#include <boost/uuid/uuid_io.hpp>

// Windows API
#include <winrt/Windows.Security.Cryptography.h>

using std::scoped_lock;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;
using winrt::array_view;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;

namespace msrn = winrt::Microsoft::ReactNative;

namespace {

constexpr Microsoft::React::Networking::IBlobResource::BlobFieldNames
    blobKeys{"blob", "blobId", "offset", "size", "type", "data"};

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

/*static*/ const IBlobResource::BlobFieldNames &IBlobResource::FieldNames() noexcept {
  return blobKeys;
}

/*static*/ shared_ptr<IBlobResource> IBlobResource::Make(
    winrt::Windows::Foundation::IInspectable const &inspectableProperties) {
  using namespace msrn;

  auto propBag = ReactPropertyBag{inspectableProperties.try_as<IReactPropertyBag>()};

  auto blobPersistor = std::make_shared<MemoryBlobPersistor>();
  auto contentHandler = std::make_shared<BlobWebSocketModuleContentHandler>(blobPersistor);
  auto requestBodyHandler = std::make_shared<BlobModuleRequestBodyHandler>(blobPersistor);
  auto responseHandler = std::make_shared<BlobModuleResponseHandler>(blobPersistor);

  auto contentHandlerPropId =
      ReactPropertyId<ReactNonAbiValue<weak_ptr<IWebSocketModuleContentHandler>>>{L"BlobModule.ContentHandler"};
  propBag.Set(contentHandlerPropId, weak_ptr<IWebSocketModuleContentHandler>{contentHandler});

  auto blobPersistorPropId = ReactPropertyId<ReactNonAbiValue<weak_ptr<IBlobPersistor>>>{L"Blob.Persistor"};
  ;
  propBag.Set(blobPersistorPropId, weak_ptr<IBlobPersistor>{blobPersistor});

  auto result = std::make_shared<DefaultBlobResource>(
      blobPersistor, contentHandler, requestBodyHandler, responseHandler, propBag);

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

  auto chars = reinterpret_cast<const char *>(data.data());
  auto view = std::string_view(chars, data.size());
  wsProxy->SendBinary(Utilities::EncodeBase64(view), socketId);
}

void DefaultBlobResource::CreateFromParts(msrn::JSValueArray &&parts, string &&blobId) noexcept /*override*/ {
  vector<uint8_t> buffer{};

  for (const auto &partItem : parts) {
    auto &part = partItem.AsObject();
    auto type = part.at(blobKeys.Type).AsString();
    if (blobKeys.Blob == type) {
      auto &blob = part.at(blobKeys.Data).AsObject();
      array_view<uint8_t const> bufferPart;
      try {
        bufferPart = m_blobPersistor->ResolveMessage(
            blob.at(blobKeys.BlobId).AsString(), blob.at(blobKeys.Offset).AsInt64(), blob.at(blobKeys.Size).AsInt64());
      } catch (const std::exception &e) {
        return m_callbacks.OnError(e.what());
      }

      buffer.reserve(buffer.size() + bufferPart.size());
      buffer.insert(buffer.end(), bufferPart.begin(), bufferPart.end());
    } else if ("string" == type) {
      auto data = part.at(blobKeys.Data).AsString();

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

  bool handlerAdded = false;
  if (auto prop = m_propertyBag.Get(propId)) {
    if (auto httpHandler = prop.Value().lock()) {
      httpHandler->AddRequestBodyHandler(m_requestBodyHandler);
      httpHandler->AddResponseHandler(m_responseHandler);
      handlerAdded = true;
    }
  }

  // #11439 - The absence of HttpModule.Proxy may be caused by a module initialization race condition.
  // Best-effort approach to set up the request/response handlers by exposing this interface to dependents
  // (i.e. IHttpResource).
  if (!handlerAdded) {
    auto propId = msrn::ReactPropertyId<msrn::ReactNonAbiValue<weak_ptr<IBlobResource>>>{L"Blob.Resource"};
    m_propertyBag.Set(propId, weak_ptr<IBlobResource>(shared_from_this()));
  }
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

void BlobWebSocketModuleContentHandler::ProcessMessage(
    string &&message,
    msrn::JSValueObject &params) noexcept /*override*/
{
  params[blobKeys.Data] = std::move(message);
}

void BlobWebSocketModuleContentHandler::ProcessMessage(
    vector<uint8_t> &&message,
    msrn::JSValueObject &params) noexcept /*override*/
{
  auto blob = msrn::JSValueObject{
      {blobKeys.Offset, 0},
      {blobKeys.Size, message.size()},
      {blobKeys.BlobId, m_blobPersistor->StoreMessage(std::move(message))}};

  params[blobKeys.Data] = std::move(blob);
  params[blobKeys.Type] = blobKeys.Blob;
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
  auto itr = data.find(blobKeys.Blob);

  return itr != data.cend() && !(*itr).second.AsString().empty();
}

msrn::JSValueObject BlobModuleRequestBodyHandler::ToRequestBody(
    msrn::JSValueObject &data,
    string &contentType) /*override*/ {
  auto type = contentType;
  auto itr = data.find(blobKeys.Type);
  if (itr != data.cend() && !(*itr).second.AsString().empty()) {
    type = (*itr).second.AsString();
  }
  if (type.empty()) {
    type = "application/octet-stream";
  }

  auto &blob = data[blobKeys.Blob].AsObject();
  auto blobId = blob[blobKeys.BlobId].AsString();
  auto bytes = m_blobPersistor->ResolveMessage(
      std::move(blobId), blob[blobKeys.Offset].AsInt64(), blob[blobKeys.Size].AsInt64());

  return {
      {blobKeys.Type, type},
      {blobKeys.Size, bytes.size()},
      {"bytes", msrn::JSValueArray(bytes.cbegin(), bytes.cend())}};
}

#pragma endregion IRequestBodyHandler

#pragma endregion BlobModuleRequestBodyHandler

#pragma region BlobModuleResponseHandler

BlobModuleResponseHandler::BlobModuleResponseHandler(shared_ptr<IBlobPersistor> blobPersistor) noexcept
    : m_blobPersistor{blobPersistor} {}

#pragma region IResponseHandler

bool BlobModuleResponseHandler::Supports(string &responseType) /*override*/ {
  return blobKeys.Blob == responseType;
}

msrn::JSValueObject BlobModuleResponseHandler::ToResponseData(vector<uint8_t> &&content) /*override*/ {
  return {
      {blobKeys.Offset, 0},
      {blobKeys.Size, content.size()},
      {blobKeys.BlobId, m_blobPersistor->StoreMessage(std::move(content))}};
}

#pragma endregion IResponseHandler

#pragma endregion BlobModuleResponseHandler

} // namespace Microsoft::React::Networking
