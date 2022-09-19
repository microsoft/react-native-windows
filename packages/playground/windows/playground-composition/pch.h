#pragma once

#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN 1
#define WINRT_LEAN_AND_MEAN 1

#include <windows.h>

#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include <unknwn.h>

#include <CppWinRTIncludes.h>
#include <winrt/Microsoft.ReactNative.h>
// Disable until we have 3rd party story for custom components
// #include <winrt/ReactNativePicker.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#pragma pop_macro("GetCurrentTime")
