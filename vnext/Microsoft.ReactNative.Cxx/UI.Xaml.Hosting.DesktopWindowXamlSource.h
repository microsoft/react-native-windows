// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifdef USE_WINUI3
#include <Microsoft.UI.Xaml.Hosting.DesktopWindowXamlSource.h>
using IDesktopWindowXamlSourceNative2 = IDesktopWindowXamlSourceNative;
#else
#include <Windows.UI.Xaml.Hosting.DesktopWindowXamlSource.h>
#endif //  USE_WINUI3
