// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "SafeLoadLibrary.h"

namespace Microsoft::ReactNative {

// Unsafe calls to LoadLibrary/LoadLibraryEx.
// The default behavior of LoadLibrary, or LoadLibraryEx without flags, is to try and find the dependency by iterating
// through a search order. This search order contains the current working directory. In the classic attack, a malicious
// DLL is dropped in the likely - controllable current working directory.The malicious DLL has the same name as a
// missing dependency or dependency that is not in the same directory as the executable.When the call to LoadLibrary is
// reached, the malicious DLL is loaded preferentially, and code execution occurs.
//
// The SafeLoadLibrary is the preferred ways to manually load dependencies.
// The API does not search the current working directory when resolving dependencies.
// It is "borrowed" from Office Mso code.

/**
      List of new flags that control where to search for DLLs. Requires KB2533623.
*/
const DWORD SafeLoadLibraryFlags = LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS |
    LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32 | LOAD_LIBRARY_SEARCH_USER_DIRS;

HMODULE SafeLoadLibrary(const wchar_t *wzFileName, HANDLE hFile, DWORD dwFlags) noexcept {
  // When calling LoadLibrary, OR in LOAD_LIBRARY_SEARCH_DEFAULT_DIRS which enables all of the
  // safe behaviors. Note that this flag is not compatible with LOAD_WITH_ALTERED_SEARCH_PATH.
  HMODULE module =
      LoadLibraryExW(wzFileName, hFile, (dwFlags | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS) & ~LOAD_WITH_ALTERED_SEARCH_PATH);
  if (module == nullptr && GetLastError() == ERROR_INVALID_PARAMETER) {
    // Could have failed with actual bad parameters or an unpatched OS without KB2533623.
    if (GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "AddDllDirectory") == nullptr) {
      // Unpatched OS, remove all new flags.
      module = LoadLibraryExW(wzFileName, hFile, dwFlags & ~SafeLoadLibraryFlags);
    }
  }

  return module;
}

} // namespace Microsoft::ReactNative
