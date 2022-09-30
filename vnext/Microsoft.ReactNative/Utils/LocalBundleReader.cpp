// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/LocalBundleReader.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include "Unicode.h"

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace Microsoft::ReactNative {

std::string GetBundleFromEmbeddedResource(winrt::hstring str) {
  winrt::Windows::Foundation::Uri uri(str);
  auto moduleName = uri.Host();
  auto path = uri.Path();
  // skip past the leading / slash
  auto resourceName = path.c_str() + 1;

  auto hmodule = GetModuleHandle(moduleName != L"" ? moduleName.c_str() : nullptr);
  if (!hmodule) {
    throw std::invalid_argument(fmt::format("Couldn't find module {}", winrt::to_string(moduleName)));
  }

  auto resource = FindResourceW(hmodule, resourceName, RT_RCDATA);
  if (!resource) {
    throw std::invalid_argument(fmt::format(
        "Couldn't find resource {} in module {}", winrt::to_string(resourceName), winrt::to_string(moduleName)));
  }

  auto hglobal = LoadResource(hmodule, resource);
  if (!hglobal) {
    throw std::invalid_argument(fmt::format(
        "Couldn't load resource {} in module {}", winrt::to_string(resourceName), winrt::to_string(moduleName)));
  }

  auto start = static_cast<char *>(LockResource(hglobal));
  if (!start) {
    throw std::invalid_argument(fmt::format(
        "Couldn't lock resource {} in module {}", winrt::to_string(resourceName), winrt::to_string(moduleName)));
  }

  auto size = SizeofResource(hmodule, resource);
  if (!size) {
    throw std::invalid_argument(fmt::format(
        "Couldn't get size of resource {} in module {}", winrt::to_string(resourceName), winrt::to_string(moduleName)));
  }

  return std::string(start, start + size);
}

std::future<std::string> LocalBundleReader::LoadBundleAsync(const std::string &bundleUri) {
  winrt::hstring str(Microsoft::Common::Unicode::Utf8ToUtf16(bundleUri));

  co_await winrt::resume_background();

  winrt::Windows::Storage::StorageFile file{nullptr};

  // Supports "ms-appx://" or "ms-appdata://"
  if (bundleUri._Starts_with("ms-app")) {
    winrt::Windows::Foundation::Uri uri(str);
    file = co_await winrt::Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri);
  } else if (bundleUri._Starts_with("resource://")) {
    co_return GetBundleFromEmbeddedResource(str);
  } else {
    file = co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(str);
  }

  // Read the buffer manually to avoid a Utf8 -> Utf16 -> Utf8 encoding
  // roundtrip.
  auto fileBuffer{co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file)};
  auto dataReader{winrt::Windows::Storage::Streams::DataReader::FromBuffer(fileBuffer)};

  // No need to use length + 1, STL guarantees that string storage is null-terminated.
  std::string script(fileBuffer.Length(), '\0');

  // Construct the array_view to slice into the first fileBuffer.Length bytes.
  // DataReader.ReadBytes will read as many bytes as are present in the
  // array_view. The backing string has fileBuffer.Length() + 1 bytes, without
  // an explicit end it will read 1 byte to many and throw.
  dataReader.ReadBytes(winrt::array_view<uint8_t>{
      reinterpret_cast<uint8_t *>(&script[0]), reinterpret_cast<uint8_t *>(&script[script.length()])});
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

} // namespace Microsoft::ReactNative
