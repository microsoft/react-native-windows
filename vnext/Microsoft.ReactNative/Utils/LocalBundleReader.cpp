// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/LocalBundleReader.h>
#include <fmt/format.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Storage.h>
#include "Unicode.h"

#if _MSC_VER <= 1913
// VC 19 (2015-2017.6) cannot optimize co_await/cppwinrt usage
#pragma optimize("", off)
#endif

namespace Microsoft::ReactNative {

std::string GetBundleFromEmbeddedResource(const winrt::Windows::Foundation::Uri &uri) {
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

namespace {

std::string BufferToString(const winrt::Windows::Storage::Streams::IBuffer &buffer) {
  std::string result(buffer.Length(), '\0');
  if (!result.empty()) {
    auto reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(buffer);
    reader.ReadBytes(winrt::array_view<uint8_t>{
        reinterpret_cast<uint8_t *>(&result[0]), reinterpret_cast<uint8_t *>(&result[result.length()])});
  }
  return result;
}

winrt::Windows::Storage::Streams::IBuffer BytesToBuffer(const void *data, uint32_t size) {
  winrt::Windows::Storage::Streams::DataWriter writer;
  auto bytes = static_cast<const uint8_t *>(data);
  writer.WriteBytes(winrt::array_view<const uint8_t>(bytes, bytes + size));
  return writer.DetachBuffer();
}

} // namespace

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::IBuffer>
LocalBundleReader::LoadBundleAsync(const std::wstring bundleUri) {
  try {
    co_await winrt::resume_background();

    winrt::Windows::Storage::StorageFile file{nullptr};

    // Supports "ms-appx://" or "ms-appdata://"
    if (bundleUri.starts_with(L"ms-app")) {
      winrt::Windows::Foundation::Uri uri(bundleUri);
      file = co_await winrt::Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(uri);
    } else if (bundleUri.starts_with(L"resource://")) {
      winrt::Windows::Foundation::Uri uri(bundleUri);
      auto bytes = GetBundleFromEmbeddedResource(uri);
      co_return BytesToBuffer(bytes.data(), static_cast<uint32_t>(bytes.size()));
    } else {
      file = co_await winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(bundleUri);
    }

    co_return co_await winrt::Windows::Storage::FileIO::ReadBufferAsync(file);
  }
  catch (winrt::hresult_error const &e) {
    throw winrt::hresult_error(e.code(), e.message());
  }
}

std::string LocalBundleReader::LoadBundle(const std::wstring &bundlePath) {
  return BufferToString(LoadBundleAsync(bundlePath).get());
}

StorageFileBigString::StorageFileBigString(const std::wstring &path) {
  m_pendingLoad = LocalBundleReader::LoadBundleAsync(path);
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
  if (m_pendingLoad) {
    m_string = BufferToString(m_pendingLoad.get());
    m_pendingLoad = nullptr;
  }
}

} // namespace Microsoft::ReactNative
