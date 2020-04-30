#pragma once

#ifdef NO_WINUI3_SUPPORT
#define XAML_NAMESPACE Windows.UI.Xaml
#else
#define XAML_NAMESPACE Microsoft.UI.Xaml
#endif

