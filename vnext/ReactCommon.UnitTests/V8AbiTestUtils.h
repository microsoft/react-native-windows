// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <SafeLoadLibrary.h>

#include <string>

// Returns the folder of the test executable, ending with a backslash.
inline std::wstring GetTestExeDir() {
  wchar_t path[MAX_PATH]{};
  DWORD size = ::GetModuleFileNameW(nullptr, path, MAX_PATH);
  std::wstring dir(path, size);
  return dir.substr(0, dir.find_last_of(L'\\') + 1);
}

// Loads v8jsi2.dll by name like production; the build copies it next to the test executable.
inline HMODULE LoadV8JsiAbiForTesting() {
  static HMODULE s_module = Microsoft::ReactNative::LoadLibraryAsPeerFirst(L"v8jsi2.dll");
  return s_module;
}
