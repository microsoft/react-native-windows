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
// kept in sync with Background. RN clients (windows-included) that
// want different backgroundColor depending on interactive states
// such as PointerOver, Focused, and Disabled, expect to handle
// these state changes in js, and re-render with different
// ViewStyle.backgroundColor to accomplish their desired visuals.
void UpdateTextControlBackgroundResourceBrushes(
    const winrt::Windows::UI::Xaml::FrameworkElement &element,
    const winrt::Media::Brush brush);

// Some XAML controls use additional resource-brushes that need to be
// kept in sync with BorderBrush. RN clients (windows-included) that
// want different borderColor depending on interactive states
// such as PointerOver, Focused, and Disabled, expect to handle
// these state changes in js, and re-render with different
// ViewStyle.color to accomplish their desired visuals.
void UpdateTextControlForegroundResourceBrushes(
    const winrt::Windows::UI::Xaml::DependencyObject object,
    const winrt::Media::Brush brush);

// Some XAML controls use additional resource-brushes that need to be
// kept in sync with BorderBrush. RN clients (windows-included) that
// want different borderColor depending on interactive states
// such as PointerOver, Focused, and Disabled, expect to handle
// these state changes in js, and re-render with different
// ViewStyle.borderColor to accomplish their desired visuals.
void UpdateTextControlBorderResourceBrushes(
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
