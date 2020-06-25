// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifdef USE_WINUI3
// This header doesn't exist in WinUI 3,
// DesktopWindowXamlSource is moved to Microsoft.UI.Xaml.Hosting.
#else
#include <winrt/Windows.UI.Xaml.Hosting.DesktopWindowXamlSource.h>
#endif //  USE_WINUI3
