// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace react::uwp {
/// <summary>
/// Implements a guard to ensure that only one XAML dll gets loaded in the current process.
/// This is important because different XAML dlls are not inter-operable.
/// It relies on APIs and types that are part of the WDK and not usually available via the UWP SDKs,
/// but the docs allow calling these APIs via GetProcAddress.
/// </summary>
struct XamlLoadState {
  enum class XamlDialect {
    Unknown = -1,
    SystemXAML = 1,
    WinUI = 2,
  };

  struct XamlVersion {
    uint16_t m_major{};
    uint16_t m_minor{};
    std::wstring m_path{};
    XamlDialect GetMode() const noexcept;
    XamlDialect GetKnownMode() const;
  };

  XamlLoadState();
  ~XamlLoadState();

#ifndef DISABLE_XAML_GUARD
  static XamlLoadState g_Instance;
#endif

  void RegisterDll(PCWSTR dllName, PCWSTR path);

 private:
  XamlVersion m_version;
  XamlDialect m_mode{XamlDialect::Unknown};
  void *m_cookie;

  static XamlVersion GetXamlVersion(const std::wstring &path) noexcept;
};
} // namespace react::uwp