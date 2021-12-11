// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include <Utils/ResourceBrushUtils.h>
#include <Utils/StandardControlResourceKeyNames.h>

namespace Microsoft::ReactNative {

DefaultBrushStore::DefaultBrushStore() {}

DefaultBrushStore &DefaultBrushStore::Instance() {
  thread_local static DefaultBrushStore instance;
  return instance;
}

void DefaultBrushStore::Reset() {
  m_defaultBorderBrush = nullptr;
}

xaml::Media::Brush DefaultBrushStore::GetDefaultBorderBrush() {
  if (!m_defaultBorderBrush) {
    m_defaultBorderBrush = xaml::Application::Current()
                               .Resources()
                               .Lookup(winrt::box_value(L"DefaultTextForegroundThemeBrush"))
                               .as<xaml::Media::Brush>();
  }

  return m_defaultBorderBrush;
}

void UpdateResourceBrush(
    const xaml::FrameworkElement &element,
    const std::wstring &resourceName,
    const xaml::Media::Brush brush) {
  const auto resources = element.Resources();
  if (resources != nullptr) {
    if (brush != nullptr) {
      resources.Insert(winrt::box_value(resourceName), brush);
    } else {
      resources.Remove(winrt::box_value(resourceName));
    }
  }
}

void UpdateToggleResourceBrush(
    const xaml::FrameworkElement &element,
    const std::wstring &resourceName,
    const xaml::Media::Brush brush) {
  // check for null pointers
  if (const auto resources = element.Resources()) {
    if (brush) {
      // check if resources has key
      if (resources.HasKey(winrt::box_value(resourceName))) {
        auto resourceBrush = resources.Lookup(winrt::box_value(resourceName));
        // if both the old Resource Brush and new Resource Brush is a SolidColorBrush, change the color of the brush to
        // support runtime changes
        auto colorBrush = brush.try_as<xaml::Media::SolidColorBrush>();
        auto resourceColorBrush = resourceBrush.try_as<xaml::Media::SolidColorBrush>();
        if (colorBrush && resourceColorBrush)
          resourceColorBrush.Color(colorBrush.Color());
        else
          resources.Insert(winrt::box_value(resourceName), brush);
      } else {
        // else, add the new brush to the resource directory (will need to reload component to see changes)
        resources.Insert(winrt::box_value(resourceName), brush);
      }
      // if the brush is null, remove the resource
    } else {
      resources.Remove(winrt::box_value(resourceName));
    }
  }
}

void UpdateTextControlBackgroundResourceBrushes(const xaml::FrameworkElement &element, const xaml::Media::Brush brush) {
  UpdateResourceBrush(element, c_textControlBackground, brush);
  UpdateResourceBrush(element, c_textControlBackgroundPointerOver, brush);
  UpdateResourceBrush(element, c_textControlBackgroundFocused, brush);
  UpdateResourceBrush(element, c_textControlBackgroundDisabled, brush);

  UpdateResourceBrush(element, c_textControlButtonForegroundPressed, brush);
}

void UpdateTextControlForegroundResourceBrushes(const xaml::FrameworkElement element, const xaml::Media::Brush brush) {
  UpdateResourceBrush(element, c_textControlForeground, brush);
  UpdateResourceBrush(element, c_textControlForegroundPointerOver, brush);
  UpdateResourceBrush(element, c_textControlForegroundFocused, brush);
  UpdateResourceBrush(element, c_textControlForegroundDisabled, brush);

  UpdateResourceBrush(element, c_textControlButtonForeground, brush);
  UpdateResourceBrush(element, c_textControlButtonForegroundPointerOver, brush);
  UpdateResourceBrush(element, c_textControlButtonBackgroundPressed, brush);
}

void UpdateTextControlBorderResourceBrushes(const xaml::FrameworkElement &element, const xaml::Media::Brush &b) {
  // Workaround for bug https://microsoft.visualstudio.com/OS/_workitems/edit/26118890.
  // Remove when the bug gets fixed.
  xaml::Media::Brush brush = b;
  if (auto solidBrush = b.as<xaml::Media::SolidColorBrush>()) {
    brush = xaml::Media::SolidColorBrush{solidBrush.Color()};
  }

  UpdateResourceBrush(element, c_textControlBorderBrush, brush);
  UpdateResourceBrush(element, c_textControlBorderBrushPointerOver, brush);
  UpdateResourceBrush(element, c_textControlBorderBrushFocused, brush);
  UpdateResourceBrush(element, c_textControlBorderBrushDisabled, brush);
}

void UpdateToggleSwitchBorderResourceBrushes(
    const xaml::Controls::ToggleSwitch &toggleSwitch,
    const xaml::Media::Brush brush) {
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchStrokeOff, brush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchStrokeOffPointerOver, brush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchStrokeOffPressed, brush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchStrokeOffDisabled, brush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchStrokeOn, brush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchStrokeOnPointerOver, brush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchStrokeOnPressed, brush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchStrokeOnDisabled, brush);
}

void UpdateToggleSwitchThumbResourceBrushes(
    const xaml::Controls::ToggleSwitch &toggleSwitch,
    const xaml::Media::Brush thumbBrush) {
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOff, thumbBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOffPointerOver, thumbBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOffPressed, thumbBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOffDisabled, thumbBrush);

  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOn, thumbBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOnPointerOver, thumbBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOnPressed, thumbBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOnDisabled, thumbBrush);
}

void UpdateToggleSwitchTrackResourceBrushesOn(
    const xaml::Controls::ToggleSwitch &toggleSwitch,
    const xaml::Media::Brush onTrackBrush) {
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchFillOn, onTrackBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchFillOnPointerOver, onTrackBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchFillOnPressed, onTrackBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchFillOnDisabled, onTrackBrush);
}

void UpdateToggleSwitchTrackResourceBrushesOff(
    const xaml::Controls::ToggleSwitch &toggleSwitch,
    const xaml::Media::Brush offTrackBrush) {
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchFillOff, offTrackBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchFillOffPointerOver, offTrackBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchFillOffPressed, offTrackBrush);
  UpdateToggleResourceBrush(toggleSwitch, c_toggleSwitchFillOffDisabled, offTrackBrush);
}

bool IsObjectATextControl(const xaml::DependencyObject &object) {
  return object.try_as<xaml::Controls::TextBox>() != nullptr ||
      object.try_as<xaml::Controls::PasswordBox>() != nullptr ||
      object.try_as<xaml::Controls::RichEditBox>() != nullptr ||
      object.try_as<xaml::Controls::AutoSuggestBox>() != nullptr;
}

void UpdateControlBackgroundResourceBrushes(const xaml::FrameworkElement &element, const xaml::Media::Brush brush) {
  if (IsObjectATextControl(element)) {
    UpdateTextControlBackgroundResourceBrushes(element, brush);
  }
}

void UpdateControlForegroundResourceBrushes(const xaml::DependencyObject object, const xaml::Media::Brush brush) {
  if (IsObjectATextControl(object)) {
    const auto element = object.try_as<xaml::FrameworkElement>();
    UpdateTextControlForegroundResourceBrushes(element, brush);
  }
}

void UpdateControlBorderResourceBrushes(const xaml::FrameworkElement &element, const xaml::Media::Brush brush) {
  if (IsObjectATextControl(element)) {
    UpdateTextControlBorderResourceBrushes(element, brush);
  } else if (const auto toggleSwitch = element.try_as<xaml::Controls::ToggleSwitch>()) {
    UpdateToggleSwitchBorderResourceBrushes(toggleSwitch, brush);
  }
}

} // namespace Microsoft::ReactNative
