// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "BaseFileReaderResource.h"

// Boost Library
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/transform_width.hpp>

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

  // https://www.boost.org/doc/libs/1_76_0/libs/serialization/doc/dataflow.html
  using namespace boost::archive::iterators;
  typedef base64_from_binary<transform_width<const char *, 6, 8>> encode_base64;
  std::ostringstream oss;
  std::copy(encode_base64(bytes.cbegin()), encode_base64(bytes.cend()), ostream_iterator<char>(oss));
  result += oss.str();

  resolver(std::move(result));
}

/*static*/ shared_ptr<IFileReaderResource> IFileReaderResource::Make(
    std::weak_ptr<IBlobPersistor> weakBlobPersistor) noexcept {
  return std::make_shared<BaseFileReaderResource>(weakBlobPersistor);
}

#pragma endregion IFileReaderResource

#pragma endregion BaseFileReaderResource

} // namespace Microsoft::React
