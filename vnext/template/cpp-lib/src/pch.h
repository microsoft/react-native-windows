#pragma once

#define NOMINMAX

#include <hstring.h>
#include <restrictederrorinfo.h>
#include <unknwn.h>
#include <windows.h>
#include <CppWinRTIncludes.h>
#if __has_include(<VersionMacros.h>)
  #include <VersionMacros.h>
#endif

#include <winrt/Microsoft.ReactNative.h>

using namespace winrt::Windows::Foundation;
