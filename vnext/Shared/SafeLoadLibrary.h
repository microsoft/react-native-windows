// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

namespace Microsoft::ReactNative {

// Safe LoadLibraryEx wrapper that must be used instead of LoadLibrary.
extern HMODULE SafeLoadLibrary(const wchar_t *fileName, DWORD flags = 0) noexcept;

// Try to load the dll from the same folder as the current DLL first,
// and if it is not found, then use the SafeLoadLibrary.
extern HMODULE LoadLibraryAsPeerFirst(const wchar_t *fileName) noexcept;

} // namespace Microsoft::ReactNative
