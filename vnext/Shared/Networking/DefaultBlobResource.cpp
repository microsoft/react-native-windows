// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "DefaultBlobResource.h"

#include <Modules/CxxModuleUtilities.h>
#include <Modules/IHttpModuleProxy.h>
#include <Modules/IWebSocketModuleProxy.h>

// Boost Libraries
#include <boost/uuid/uuid_io.hpp>

// Windows API
#include <winrt/Windows.Security.Cryptography.h>

// Standard Library
#include <mutex>

using std::scoped_lock;
using std::shared_ptr;
using std::string;
using std::vector;
using std::weak_ptr;
using winrt::array_view;
using winrt::Windows::Security::Cryptography::CryptographicBuffer;

namespace msrn = winrt::Microsoft::ReactNative;

namespace {

constexpr char s_moduleName[] = "BlobModule";
constexpr char blobKey[] = "blob";
constexpr char blobIdKey[] = "blobId";
constexpr char offsetKey[] = "offset";
constexpr char sizeKey[] = "size";
constexpr char typeKey[] = "type";
constexpr char dataKey[] = "data";

} // namespace

namespace Microsoft::React::Networking {

#pragma region DefaultBlobResource

#pragma region IBlobResource

/*static*/ shared_ptr<IBlobResource> IBlobResource::Make(
    winrt::Windows::Foundation::IInspectable const &inspectableProperties) {
  auto result = std::make_shared<DefaultBlobResource>();

  return result;
}

void DefaultBlobResource::SendOverSocket(string &&blobId, int64_t offset, int64_t size, int64_t socketId) noexcept
/*override*/ {
  auto propBag = msrn::ReactPropertyBag{}; // TODO: Fetch from context.
  auto propId =
      msrn::ReactPropertyId<msrn::ReactNonAbiValue<weak_ptr<IWebSocketModuleProxy>>>{L"WebSocketModule.Proxy"};
  shared_ptr<IWebSocketModuleProxy> wsProxy;
  if (auto prop = propBag.Get(propId)) {
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

} // namespace Microsoft::React::Networking
