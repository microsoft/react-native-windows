// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

namespace Microsoft::ReactNative {

// Safe LoadLibraryEx wrapper that must be used instead of LoadLibrary.
extern HMODULE SafeLoadLibrary(const wchar_t *wzFileName, HANDLE hFile = nullptr, DWORD dwFlags = 0) noexcept;

} // namespace Microsoft::ReactNative
