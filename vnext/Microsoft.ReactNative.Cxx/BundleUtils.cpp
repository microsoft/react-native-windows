// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "BundleUtils.h"

#include <winrt/Windows.Foundation.h>
#include <fmt/format.h>

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

} // namespace Microsoft::ReactNative
