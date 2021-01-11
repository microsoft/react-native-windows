// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "XamlLoadState.h"
#include <sstream>
#include "Unicode.h"

#ifndef DISABLE_XAML_GUARD

using namespace react::uwp;

constexpr std::wstring_view systemXamlDllName{L"Windows.UI.Xaml.dll"};
constexpr std::wstring_view winUIDllName{L"Microsoft.UI.Xaml.dll"};

std::ostream &operator<<(std::ostream &o, const XamlLoadState::XamlVersion &version) {
  return o << version.m_major << "." << version.m_minor;
}

XamlLoadState::XamlDialect XamlLoadState::XamlVersion::GetMode() const noexcept {
  switch (m_major) {
    case 2:
      return XamlDialect::SystemXAML;
      break;
    case 3:
      return XamlDialect::WinUI;
      break;
    default:
      return XamlDialect::Unknown;
      break;
  }
}

XamlLoadState::XamlDialect XamlLoadState::XamlVersion::GetKnownMode() const {
  auto const mode = GetMode();
  if (mode == XamlDialect::Unknown) {
    std::stringstream ss;
    ss << "Unknown XAML version " << *this;
    if (!m_path.empty()) {
      ss << " found at " << Microsoft::Common::Unicode::Utf16ToUtf8(m_path);
    }
    throw std::exception(ss.str().c_str());
  }
  return mode;
}

#pragma region NT APIs
// See:
//  https://docs.microsoft.com/en-us/windows/win32/devnotes/ldrregisterdllnotification
//  https://docs.microsoft.com/en-us/windows/win32/devnotes/ldrunregisterdllnotification
//  https://docs.microsoft.com/en-us/windows/win32/api/ntdef/ns-ntdef-_unicode_string

typedef LONG NTSTATUS;

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef UNICODE_STRING const *PCUNICODE_STRING;

typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {
  ULONG Flags; // Reserved.
  PCUNICODE_STRING FullDllName; // The full path name of the DLL module.
  PCUNICODE_STRING BaseDllName; // The base file name of the DLL module.
  PVOID DllBase; // A pointer to the base address for the DLL in memory.
  ULONG SizeOfImage; // The size of the DLL image, in bytes.
} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;

typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA {
  ULONG Flags; // Reserved.
  PCUNICODE_STRING FullDllName; // The full path name of the DLL module.
  PCUNICODE_STRING BaseDllName; // The base file name of the DLL module.
  PVOID DllBase; // A pointer to the base address for the DLL in memory.
  ULONG SizeOfImage; // The size of the DLL image, in bytes.
} LDR_DLL_UNLOADED_NOTIFICATION_DATA, *PLDR_DLL_UNLOADED_NOTIFICATION_DATA;

typedef union _LDR_DLL_NOTIFICATION_DATA {
  LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
  LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
} LDR_DLL_NOTIFICATION_DATA, *PLDR_DLL_NOTIFICATION_DATA;

typedef LDR_DLL_NOTIFICATION_DATA const *PCLDR_DLL_NOTIFICATION_DATA;

typedef VOID CALLBACK LDR_DLL_NOTIFICATION_FUNCTION(
    _In_ ULONG NotificationReason,
    _In_ PCLDR_DLL_NOTIFICATION_DATA NotificationData,
    _In_opt_ PVOID Context);

typedef LDR_DLL_NOTIFICATION_FUNCTION *PLDR_DLL_NOTIFICATION_FUNCTION;

#define LDR_DLL_NOTIFICATION_REASON_LOADED 1

NTSTATUS NTAPI LdrRegisterDllNotification(
    _In_ ULONG Flags,
    _In_ PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction,
    _In_opt_ PVOID Context,
    _Out_ PVOID *Cookie);

NTSTATUS NTAPI LdrUnregisterDllNotification(_In_ PVOID Cookie);

#pragma endregion

LDR_DLL_NOTIFICATION_FUNCTION XamlLoadNotification;

#define NTDLL_FUNCTION(name) reinterpret_cast<decltype(&name)>(GetProcAddress(ntdll, #name))

XamlLoadState::XamlVersion XamlLoadState::GetXamlVersion(const std::wstring &path) noexcept {
  DWORD dwHandle;
  DWORD size = GetFileVersionInfoSizeExW(0, path.c_str(), &dwHandle);
  std::string buffer;
  buffer.reserve(size);

  if (GetFileVersionInfoExW(0, path.c_str(), 0, size, const_cast<char *>(buffer.c_str()))) {
    VS_FIXEDFILEINFO *fileInfo;
    UINT len{};
    if (VerQueryValueW(buffer.c_str(), L"\\", (void **)&fileInfo, &len)) {
      const DWORD ls = fileInfo->dwFileVersionMS & 0xffff;
      DWORD ms = fileInfo->dwFileVersionMS >> 16;
      // WinUI 3 Alpha has version 10.0.10011.16384. Convert 10.0 to 3.0
      if (ms == 10) {
        ms = 3;
      }
      return {static_cast<uint16_t>(ms), static_cast<uint16_t>(ls), path};
    }
  }

  return {};
}

XamlLoadState::XamlLoadState() {
  HMODULE mod{nullptr};

  if (GetModuleHandle(systemXamlDllName.data())) {
    RegisterDll(systemXamlDllName.data(), nullptr);
  }
  mod = GetModuleHandle(winUIDllName.data());
  if (mod) {
    wchar_t path[MAX_PATH]{};
    if (GetModuleFileNameW(mod, path, ARRAYSIZE(path))) {
      RegisterDll(winUIDllName.data(), path);
    }
  }

  auto ntdll = GetModuleHandle(L"ntdll.dll");
  _Analysis_assume_(ntdll != nullptr);
  auto pfn = NTDLL_FUNCTION(LdrRegisterDllNotification);
  (*pfn)(0, XamlLoadNotification, nullptr, &m_cookie);
}

XamlLoadState::~XamlLoadState() {
  auto ntdll = GetModuleHandle(L"ntdll.dll");
  _Analysis_assume_(ntdll != nullptr);
  auto pfn = NTDLL_FUNCTION(LdrUnregisterDllNotification);
  (*pfn)(m_cookie);
}

void XamlLoadState::RegisterDll(PCWSTR dllName, PCWSTR path) {
  if (dllName == systemXamlDllName) {
    switch (m_mode) {
      case XamlDialect::Unknown:
        m_mode = XamlDialect::SystemXAML;
        return;
      case XamlDialect::SystemXAML:
        // should never get here
        return;
      case XamlDialect::WinUI:
        throw std::exception("XAML dialect mismatch - WinUI 3 already loaded when attempting to load Windows.UI.Xaml");
    }
  } else if (dllName == winUIDllName) {
    auto newVersion = GetXamlVersion(path);
    auto const newMode = newVersion.GetKnownMode();
    switch (m_mode) {
      case XamlDialect::Unknown:
        m_version = newVersion;
        m_mode = newMode;
        break;

      case XamlDialect::SystemXAML:
        if ((m_version.GetMode() == XamlDialect::Unknown) && (newMode == XamlDialect::SystemXAML)) {
          m_version = newVersion;
        } else {
          /* we've either:
             -  loaded WUX and we're trying to load WinUI 3
             -  loaded WinUI 2.x and we're trying to load WinUI 3 or a different WinUI 2.x
             */
          throw std::exception("XAML dialect mismatch");
        }
        break;

      case XamlDialect::WinUI:
        throw std::exception("WinUI 3 already loaded");
        break;

      default:
        std::stringstream ss;
        ss << "Unexpected XAML mode " << static_cast<int>(m_mode) << " when trying to load " << (path ? path : dllName);
        throw std::exception(ss.str().c_str());
    }
  }
}

XamlLoadState XamlLoadState::g_Instance;

VOID CALLBACK XamlLoadNotification(ULONG reason, PCLDR_DLL_NOTIFICATION_DATA data, void *context) {
  if (reason == LDR_DLL_NOTIFICATION_REASON_LOADED) {
    auto name = data->Loaded.BaseDllName->Buffer;
    auto path = data->Loaded.FullDllName->Buffer;
    if (name != nullptr) {
      XamlLoadState::g_Instance.RegisterDll(name, path);
    }
  }
}

#endif // DEBUG
