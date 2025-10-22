// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BaseFileReaderResource.h"

#include <utilities.h>
#include "InputValidation.h"

// Windows API
#include <winrt/base.h>

using std::function;
using std::shared_ptr;
using std::string;

namespace Microsoft::React {

#pragma region BaseFileReaderResource

BaseFileReaderResource::BaseFileReaderResource(std::weak_ptr<IBlobPersistor> weakBlobPersistor) noexcept
    : m_weakBlobPersistor{weakBlobPersistor} {}

#pragma region IFileReaderResource

void BaseFileReaderResource::ReadAsText(
    string &&blobId,
    int64_t offset,
    int64_t size,
    string &&encoding,
    function<void(string &&)> &&resolver,
    function<void(string &&)> &&rejecter) noexcept /*override*/ {
  //  VALIDATE Blob ID - PATH TRAVERSAL PROTECTION (P0 Critical - CVSS 8.6)
  try {
    Microsoft::ReactNative::InputValidation::PathValidator::ValidateBlobId(blobId);

    //  VALIDATE Size - DoS PROTECTION
    if (size > 0) {
      Microsoft::ReactNative::InputValidation::SizeValidator::ValidateSize(
          static_cast<size_t>(size),
          Microsoft::ReactNative::InputValidation::SizeValidator::MAX_BLOB_SIZE,
          "FileReader blob"
      );
    }
  } catch (const Microsoft::ReactNative::InputValidation::ValidationException& ex) {
    return rejecter(ex.what());
  }

  auto persistor = m_weakBlobPersistor.lock();
  if (!persistor) {
    return resolver("Could not find Blob persistor");
  }

  winrt::array_view<uint8_t const> bytes;
  try {
    bytes = persistor->ResolveMessage(std::move(blobId), offset, size);
  } catch (const std::exception &e) {
    return rejecter(e.what());
  }

  // #9982 - Handle non-UTF8 encodings
  //         See https://docs.oracle.com/en/java/javase/11/docs/api/java.base/java/nio/charset/Charset.html
  auto result = string{bytes.cbegin(), bytes.cend()};

  resolver(std::move(result));
}

void BaseFileReaderResource::ReadAsDataUrl(
    string &&blobId,
    int64_t offset,
    int64_t size,
    string &&type,
    function<void(string &&)> &&resolver,
    function<void(string &&)> &&rejecter) noexcept /*override*/ {
  //  VALIDATE Blob ID - PATH TRAVERSAL PROTECTION (P0 Critical - CVSS 8.6)
  try {
    Microsoft::ReactNative::InputValidation::PathValidator::ValidateBlobId(blobId);

    //  VALIDATE Size - DoS PROTECTION
    if (size > 0) {
      Microsoft::ReactNative::InputValidation::SizeValidator::ValidateSize(
          static_cast<size_t>(size),
          Microsoft::ReactNative::InputValidation::SizeValidator::MAX_BLOB_SIZE,
          "FileReader data URL blob"
      );
    }
  } catch (const Microsoft::ReactNative::InputValidation::ValidationException& ex) {
    return rejecter(ex.what());
  }

  auto persistor = m_weakBlobPersistor.lock();
  if (!persistor) {
    return rejecter("Could not find Blob persistor");
  }

  winrt::array_view<uint8_t const> bytes;
  try {
    bytes = persistor->ResolveMessage(std::move(blobId), offset, size);
  } catch (const std::exception &e) {
    return rejecter(e.what());
  }

  auto result = string{"data:"};
  result += type;
  result += ";base64,";

  auto chars = reinterpret_cast<const char *>(bytes.data());
  auto view = std::string_view(chars, bytes.size());
  result += Utilities::EncodeBase64(view);

  resolver(std::move(result));
}

/*static*/ shared_ptr<IFileReaderResource> IFileReaderResource::Make(
    std::weak_ptr<IBlobPersistor> weakBlobPersistor) noexcept {
  return std::make_shared<BaseFileReaderResource>(weakBlobPersistor);
}

#pragma endregion IFileReaderResource

#pragma endregion BaseFileReaderResource

} // namespace Microsoft::React
