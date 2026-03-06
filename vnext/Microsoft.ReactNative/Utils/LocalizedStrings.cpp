// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <windows.h>
#include "LocalizedStrings.h"

namespace Microsoft::ReactNative {

namespace {

HMODULE GetCurrentModule() noexcept {
  static HMODULE s_module = [] {
    HMODULE hmod = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&GetCurrentModule),
        &hmod);
    return hmod;
  }();
  return s_module;
}

} // namespace

std::wstring GetLocalizedString(UINT stringId) noexcept {
  LPCWSTR buffer = nullptr;
  int length = LoadStringW(GetCurrentModule(), stringId, reinterpret_cast<LPWSTR>(&buffer), 0);
  if (length > 0 && buffer) {
    return std::wstring(buffer, static_cast<size_t>(length));
  }
  return {};
}

} // namespace Microsoft::ReactNative
