#pragma once

#define NOMINMAX 1
#define WIN32_LEAN_AND_MEAN 1
#define WINRT_LEAN_AND_MEAN 1

#include <windows.h>

#pragma push_macro("GetCurrentTime")
#undef GetCurrentTime

#include <unknwn.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>

#include <Windows.UI.Xaml.Hosting.DesktopWindowXamlSource.h>

#pragma pop_macro("GetCurrentTime")
