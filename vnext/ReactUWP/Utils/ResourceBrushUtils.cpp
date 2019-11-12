// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ResourceBrushUtils.h"

#include <winrt/Windows.UI.Xaml.h>


namespace winrt {
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Xaml::Controls;
}

namespace react {
namespace uwp {


void UpdateTextControlBackgroundResourceBrushes(
    const winrt::FrameworkElement &element,
    const winrt::Brush brush) {
  if (IsObjectATextControl(element)) {
    UpdateResourceBrush(element, L"TextControlBackground", brush);
    UpdateResourceBrush(element, L"TextControlBackgroundPointerOver", brush);
    UpdateResourceBrush(element, L"TextControlBackgroundFocused", brush);
    UpdateResourceBrush(element, L"TextControlBackgroundDisabled", brush);

    UpdateResourceBrush(element, L"TextControlButtonForegroundPressed", brush);
  }
}

void UpdateTextControlForegroundResourceBrushes(
    const winrt::DependencyObject object,
    const winrt::Brush brush) {
  if (IsObjectATextControl(object)) {
    TryUpdateResourceBrush(object, L"TextControlForeground", brush);
    TryUpdateResourceBrush(object, L"TextControlForegroundPointerOver", brush);
    TryUpdateResourceBrush(object, L"TextControlForegroundFocused", brush);
    TryUpdateResourceBrush(object, L"TextControlForegroundDisabled", brush);

    TryUpdateResourceBrush(object, L"TextControlButtonForeground", brush);
    TryUpdateResourceBrush(object, L"TextControlButtonForegroundPointerOver", brush);
    TryUpdateResourceBrush(object, L"TextControlButtonBackgroundPressed", brush);
  }
}

void UpdateTextControlBorderResourceBrushes(
    const winrt::FrameworkElement &element,
    const winrt::Brush brush) {
  if (IsObjectATextControl(element)) {
    UpdateResourceBrush(element, L"TextControlBorderBrush", brush);
    UpdateResourceBrush(element, L"TextControlBorderBrushPointerOver", brush);
    UpdateResourceBrush(element, L"TextControlBorderBrushFocused", brush);
    UpdateResourceBrush(element, L"TextControlBorderBrushDisabled", brush);
  }
}

void TryUpdateResourceBrush(
    const winrt::DependencyObject &object,
    const std::wstring &resourceName,
    const winrt::Brush brush) {
  const auto element = object.try_as<winrt::Windows::UI::Xaml::FrameworkElement>();
  if (element != nullptr) {
    UpdateResourceBrush(element, resourceName, brush);
  }
}

void UpdateResourceBrush(
    const winrt::FrameworkElement &element,
    const std::wstring &resourceName,
    const winrt::Brush brush) {
  const auto resources = element.Resources();
  if (resources != nullptr) {
    if (brush != nullptr) {
      resources.Insert(winrt::box_value(resourceName), brush);
    } else {
      resources.Remove(winrt::box_value(resourceName));
    }
  }
}

bool IsObjectATextControl(const winrt::DependencyObject &object) {
  return object.try_as<winrt::TextBox>() != nullptr ||
      object.try_as<winrt::PasswordBox>() != nullptr ||
      object.try_as<winrt::RichEditBox>() != nullptr ||
      object.try_as<winrt::AutoSuggestBox>() != nullptr;
}

} // namespace uwp
} // namespace react
