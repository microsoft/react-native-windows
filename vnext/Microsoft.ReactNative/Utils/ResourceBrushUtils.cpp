// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/ResourceBrushUtils.h>
#include <Utils/StandardControlResourceKeyNames.h>

namespace react::uwp {

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
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOff, thumbBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOffPointerOver, thumbBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOffPressed, thumbBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOffDisabled, thumbBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOn, thumbBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOnPointerOver, thumbBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOnPressed, thumbBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchKnobFillOnDisabled, thumbBrush);
}

void UpdateToggleSwitchTrackResourceBrushes(
    const xaml::Controls::ToggleSwitch &toggleSwitch,
    const xaml::Media::Brush onTrackBrush,
    const xaml::Media::Brush offTrackBrush) {
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchFillOn, onTrackBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchFillOnPointerOver, onTrackBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchFillOnPressed, onTrackBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchFillOnDisabled, onTrackBrush);

  UpdateResourceBrush(toggleSwitch, c_toggleSwitchFillOff, offTrackBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchFillOffPointerOver, offTrackBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchFillOffPressed, offTrackBrush);
  UpdateResourceBrush(toggleSwitch, c_toggleSwitchFillOffDisabled, offTrackBrush);
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

} // namespace react::uwp
