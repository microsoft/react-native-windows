#include "pch.h"
#include "XamlLoadState.h"
#include <sstream>

XamlLoadState XamlLoadState::g_Instance;

constexpr std::wstring_view wux{L"Windows.UI.Xaml.dll"};
constexpr std::wstring_view mux{L"Microsoft.UI.Xaml.dll"};

#pragma region NT APIs

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

LDR_DLL_NOTIFICATION_FUNCTION MyNotificationFunction;

#define NTDLL_FUNCTION(name) reinterpret_cast<decltype(&name)>(GetProcAddress(ntdll, #name))

std::string XamlLoadState::GetXamlVersion(PCWSTR path) {
  DWORD dwHandle;
  DWORD size = GetFileVersionInfoSizeW(path, &dwHandle);
  std::string buffer;
  buffer.reserve(size);

  if (GetFileVersionInfo(path, 0, size, const_cast<char *>(buffer.c_str()))) {
    VS_FIXEDFILEINFO *fileInfo;
    UINT len{};
    if (VerQueryValueW(buffer.c_str(), L"\\", (void **)&fileInfo, &len)) {
      const DWORD ls = fileInfo->dwFileVersionMS & 0x3fff;
      DWORD ms = fileInfo->dwFileVersionMS >> 16;
      // WinUI 3 Alpha has version 10.0.10011.16384. Convert 10.0 to 3.0
      if (ms == 10) {
        ms = 3;
      }
      std::stringstream v;
      v << ms << "." << ls;
      return v.str();
    }
  }

  return {};
}

XamlLoadState::XamlLoadState() {
  HMODULE mod{nullptr};

  if (GetModuleHandle(wux.data())) {
    RegisterDll(wux.data(), nullptr);
  }
  mod = GetModuleHandle(mux.data());
  if (mod) {
    wchar_t path[MAX_PATH]{};
    if (GetModuleFileNameW(mod, path, ARRAYSIZE(path))) {
      RegisterDll(mux.data(), path);
    }
  }

  auto ntdll = GetModuleHandle(L"ntdll.dll");
  auto pfn = NTDLL_FUNCTION(LdrRegisterDllNotification);
  (*pfn)(0, MyNotificationFunction, nullptr, &m_cookie);
}

XamlLoadState::~XamlLoadState() {
  auto ntdll = GetModuleHandle(L"ntdll.dll");
  auto pfn = NTDLL_FUNCTION(LdrUnregisterDllNotification);
  (*pfn)(m_cookie);
}

void XamlLoadState::RegisterDll(PCWSTR DllName, PCWSTR path) {
  if (wux == DllName) {
    switch (m_mode) {
      case XamlDialect::Unknown:
        m_mode = XamlDialect::SystemXAML;
        return;
      case XamlDialect::SystemXAML:
        // should never get here
        return;
      case XamlDialect::WinUI:
        throw std::exception("XAML dialect mismatch - WinUI 3 already loaded when attempting to load WUX");
    }
  } else if (mux == DllName) {
    std::string newVersion = GetXamlVersion(path);
    switch (m_mode) {
      case XamlDialect::Unknown:
        m_version = newVersion;
        switch (m_version[0]) {
          case '2':
            m_mode = XamlDialect::SystemXAML;
            break;
          case '3':
            m_mode = XamlDialect::WinUI;
            break;
          default:
            throw std::exception("Unknown XAML version");
            break;
        }
        break;

      case XamlDialect::SystemXAML:
        if (m_version == "" && newVersion[0] == '2') {
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
        throw std::exception("Unexpected");
    }
  }
}

VOID CALLBACK MyNotificationFunction(ULONG reason, PCLDR_DLL_NOTIFICATION_DATA data, void *context) {
  if (reason == LDR_DLL_NOTIFICATION_REASON_LOADED) {
    auto name = data->Loaded.BaseDllName->Buffer;
    auto path = data->Loaded.FullDllName->Buffer;
    if (name != nullptr) {
      XamlLoadState::g_Instance.RegisterDll(name, path);
    }
  }
}
