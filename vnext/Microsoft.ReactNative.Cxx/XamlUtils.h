// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

//
// Common XAML utilities.
//

#include "CppWinRTIncludes.h"

extern "C" {
HMODULE
WINAPI
WINRT_IMPL_GetModuleHandleW(_In_opt_ LPCWSTR lpModuleName);
}

#ifndef WINRT_IMPL_LINK
#ifdef _M_HYBRID
#define WINRT_IMPL_LINK(function, count) \
  __pragma(comment(linker, "/alternatename:#WINRT_IMPL_" #function "@" #count "=#" #function "@" #count))
#elif _M_ARM64EC
#define WINRT_IMPL_LINK(function, count) \
  __pragma(comment(linker, "/alternatename:#WINRT_IMPL_" #function "=#" #function))
#elif _M_IX86
#define WINRT_IMPL_LINK(function, count) \
  __pragma(comment(linker, "/alternatename:_WINRT_IMPL_" #function "@" #count "=_" #function "@" #count))
#else
#define WINRT_IMPL_LINK(function, count) __pragma(comment(linker, "/alternatename:WINRT_IMPL_" #function "=" #function))
#endif
#endif
WINRT_IMPL_LINK(GetModuleHandleW, 4);
#undef WINRT_IMPL_LINK

namespace XAML_CPPWINRT_NAMESPACE {

// Using Windows::UI::ColorHelper causes the process to load Windows.UI.Xaml.dll which is not needed just to fill a
// Color struct
inline winrt::Windows::UI::Color FromArgb(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
  return winrt::Windows::UI::Color{a, r, g, b};
}
} // namespace XAML_CPPWINRT_NAMESPACE
