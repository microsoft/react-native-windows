// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.h>

namespace winrt {
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Xaml::Media;
}

namespace react {
namespace uwp {

// Some XAML controls use additional resource-brushes that need to be
// kept in sync with the Background, Foreground, and BorderBrush
// DependencyProperties. RN clients (windows-included) that
// want different backgroundColor, color, or borderColor
// depending on interactive states (e.g. PointerOver, Focused,
// and Disabled), expect to handle these state changes in js by
// re-rendering with different props in response to state
// change events (onFocus, onBlur, onMouseEnter, onMouseLeave)
// or when changing enabled/disabled props.
void UpdateControlBackgroundResourceBrushes(
    const winrt::Windows::UI::Xaml::FrameworkElement &element,
    const winrt::Media::Brush brush);

void UpdateControlForegroundResourceBrushes(
    const winrt::Windows::UI::Xaml::DependencyObject object,
    const winrt::Media::Brush brush);

void UpdateControlBorderResourceBrushes(
    const winrt::Windows::UI::Xaml::FrameworkElement &element,
    const winrt::Media::Brush brush);

void TryUpdateResourceBrush(
    const winrt::Windows::UI::Xaml::DependencyObject &object,
    const std::wstring &resourceName,
    const winrt::Windows::UI::Xaml::Media::Brush brush);

void UpdateResourceBrush(
    const winrt::Windows::UI::Xaml::FrameworkElement &element,
    const std::wstring &resourceName,
    const winrt::Windows::UI::Xaml::Media::Brush brush);

bool IsObjectATextControl(const winrt::Windows::UI::Xaml::DependencyObject &object);

}
} // namespace react
