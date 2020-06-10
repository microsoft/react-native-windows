// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "RangeAutomationPeer.h"
#include "DynamicAutomationProperties.h"

#include <UI.Xaml.Controls.h>
#include <UI.Xaml.Controls.Primitives.h>

// Needed for latest versions of C++/WinRT
#if __has_include("RangeAutomationPeer.g.cpp")
#include "RangeAutomationPeer.g.cpp"
#endif

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

RangeAutomationPeer::RangeAutomationPeer(xaml::UIElement const &uiElement) : m_uiElement(uiElement) {}

// IRangeValueProvider
double RangeAutomationPeer::Minimum() {
  return GetAccessibilityValueRange(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue::Min);
}

double RangeAutomationPeer::Maximum() {
  return GetAccessibilityValueRange(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue::Max);
}

double RangeAutomationPeer::Value() {
  return GetAccessibilityValueRange(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue::Now);
}

// Controls such as Slider, ProgressBar, ScrollBar are by definition interactive.
bool RangeAutomationPeer::IsReadOnly() {
  return false;
}

void RangeAutomationPeer::SetValue(double value) {
  DynamicAutomationProperties::DispatchAccessibilityAction(m_uiElement, L"setValue");
}

// The following methods are not React Native props, but we need them to implement IRangeValueProvider.
double RangeAutomationPeer::SmallChange() {
  if (const auto& rangeElement = m_uiElement.try_as<xaml::Controls::Primitives::RangeBase>()) {
    return rangeElement.SmallChange();
  }

  return 0;
}

// TODO check if this gets set automatically
double RangeAutomationPeer::LargeChange() {
  if (const auto &rangeElement = m_uiElement.try_as<xaml::Controls::Primitives::RangeBase>()) {
    return rangeElement.LargeChange();
  }

  return 0;
}


// Private Methods
double RangeAutomationPeer::GetAccessibilityValueRange(
    winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue accValue) const {
  try {
    if (m_uiElement) {
      switch (accValue) {
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue::Min:
          return DynamicAutomationProperties::GetAccessibilityValueMin(m_uiElement);
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue::Max:
          return DynamicAutomationProperties::GetAccessibilityValueMax(m_uiElement);
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue::Now:
          return DynamicAutomationProperties::GetAccessibilityValueNow(m_uiElement);
      }
    }
  } catch (...) {
  }

  return 0;
}
} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation
