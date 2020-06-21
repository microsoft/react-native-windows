#pragma once

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
  std::string m_version{};
  XamlDialect m_mode{XamlDialect::Unknown};
  void *m_cookie;

  void RegisterDll(PCWSTR DllName, PCWSTR path);
  static std::string GetXamlVersion(PCWSTR path);
  XamlLoadState();
  ~XamlLoadState();

  static XamlLoadState g_Instance;
};
