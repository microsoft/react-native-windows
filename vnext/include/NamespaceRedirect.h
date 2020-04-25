#pragma once

#ifndef WINUI3_SUPPORT
#define XAML_NAMESPACE Windows.UI.Xaml
#else
#define XAML_NAMESPACE Microsoft.UI.Xaml
#endif

#define CONCAT_(X, Y) X##Y
#define CONCAT(X, Y) CONCAT_(X, Y)
#define MAKE_CONCAT(X) CONCAT(XAML_NAMESPACE, X)
#define DOT .

/// WUXMUX_TYPE(x) generates a the equivalent of Windows.UI.Xaml.x or Microsoft.UI.Xaml.x,
// depending on whether WINUI3_SUPPORT is defined or not.
#define WUXMUX_TYPE(X) MAKE_CONCAT(CONCAT(DOT, X))
