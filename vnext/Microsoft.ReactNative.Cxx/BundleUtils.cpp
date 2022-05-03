// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "BundleUtils.h"

#include <fmt/format.h>
#include <winrt/Windows.Security.Cryptography.h>

namespace Microsoft::ReactNative {

winrt::array_view<char> GetEmbeddedResource(const winrt::hstring &str) {
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

  return winrt::array_view<char>(start, size);
}

winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
GetImageInlineDataAsync(winrt::hstring uri) {
  size_t start = std::wstring_view(uri).find(L',');
  if (start == std::string::npos || start + 1 > uri.size())
    co_return nullptr;

  try {
    co_await winrt::resume_background();

    std::wstring_view base64String = uri;
    base64String.remove_prefix(start + 1);
    auto buffer = winrt::Windows::Security::Cryptography::CryptographicBuffer::DecodeFromBase64String(base64String);

    winrt::Windows::Storage::Streams::InMemoryRandomAccessStream memoryStream;
    co_await memoryStream.WriteAsync(buffer);
    memoryStream.Seek(0);

    co_return memoryStream;
  } catch (winrt::hresult_error const &) {
    // Base64 decode failed
  }

  co_return nullptr;
}

} // namespace Microsoft::ReactNative
