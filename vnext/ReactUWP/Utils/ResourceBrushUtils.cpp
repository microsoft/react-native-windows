// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ResourceBrushUtils.h"
#include "StandardControlResourceKeyNames.h"

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
    UpdateResourceBrush(element, c_textControlBackground, brush);
    UpdateResourceBrush(element, c_textControlBackgroundPointerOver, brush);
    UpdateResourceBrush(element, c_textControlBackgroundFocused, brush);
    UpdateResourceBrush(element, c_textControlBackgroundDisabled, brush);

    UpdateResourceBrush(element, c_textControlButtonForegroundPressed, brush);
  }
}

void UpdateTextControlForegroundResourceBrushes(
    const winrt::DependencyObject object,
    const winrt::Brush brush) {
  if (IsObjectATextControl(object)) {
    TryUpdateResourceBrush(object, c_textControlForeground, brush);
    TryUpdateResourceBrush(object, c_textControlForegroundPointerOver, brush);
    TryUpdateResourceBrush(object, c_textControlForegroundFocused, brush);
    TryUpdateResourceBrush(object, c_textControlForegroundDisabled, brush);

    TryUpdateResourceBrush(object, c_textControlButtonForeground, brush);
    TryUpdateResourceBrush(object, c_textControlButtonForegroundPointerOver, brush);
    TryUpdateResourceBrush(object, c_textControlButtonBackgroundPressed, brush);
  }
}

void UpdateTextControlBorderResourceBrushes(
    const winrt::FrameworkElement &element,
    const winrt::Brush brush) {
  if (IsObjectATextControl(element)) {
    UpdateResourceBrush(element, c_textControlBorderBrush, brush);
    UpdateResourceBrush(element, c_textControlBorderBrushPointerOver, brush);
    UpdateResourceBrush(element, c_textControlBorderBrushFocused, brush);
    UpdateResourceBrush(element, c_textControlBorderBrushDisabled, brush);
  }
}

void TryUpdateResourceBrush(
    const winrt::DependencyObject &object,
    const std::wstring &resourceName,
    const winrt::Brush brush) {
  const auto element = object.try_as<winrt::FrameworkElement>();
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
