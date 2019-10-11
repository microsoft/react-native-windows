// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include "LocalBundleReader.h"
#include "Unicode.h"

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace react {
namespace uwp {

std::future<std::string> LocalBundleReader::LoadBundleAsync(
    const std::string &bundleUri) {
  winrt::hstring str(Microsoft::Common::Unicode::Utf8ToUtf16(bundleUri));
  winrt::Windows::Foundation::Uri uri(str);

  co_await winrt::resume_background();

  auto file = co_await
      winrt::Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri);

  // Read the buffer manually to avoid a Utf8 -> Utf16 -> Utf8 encoding
  // roundtrip.
  auto fileBuffer{
      co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file)};
  auto dataReader{
      winrt::Windows::Storage::Streams::DataReader::FromBuffer(fileBuffer)};

  std::string script(fileBuffer.Length() + 1, '\0');

  // Construct the array_view to slice into the first fileBuffer.Length bytes.
  // DataReader.ReadBytes will read as many bytes as are present in the
  // array_view. The backing string has fileBuffer.Length() + 1 bytes, without
  // an explicit end it will read 1 byte to many and throw.
  dataReader.ReadBytes(winrt::array_view<uint8_t>{
      reinterpret_cast<uint8_t *>(&script[0]),
      reinterpret_cast<uint8_t *>(&script[script.length() - 1])});
  dataReader.Close();

  co_return script;
}

std::string LocalBundleReader::LoadBundle(const std::string &bundlePath) {
  return LoadBundleAsync(bundlePath).get();
}

StorageFileBigString::StorageFileBigString(const std::string &path) {
  m_futureBuffer = LocalBundleReader::LoadBundleAsync(path);
}

bool StorageFileBigString::isAscii() const {
  return false;
}

const char *StorageFileBigString::c_str() const {
  ensure();
  return m_string.c_str();
}

size_t StorageFileBigString::size() const {
  ensure();
  return m_string.size();
}

void StorageFileBigString::ensure() const {
  if (m_string.empty()) {
    m_string = m_futureBuffer.get();
  }
}

} // namespace uwp
} // namespace react
