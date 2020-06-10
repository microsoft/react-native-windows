// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationProperties.h"
#include "ValueAutomationPeer.h"

#include <UI.Xaml.Controls.h>

// Needed for latest versions of C++/WinRT
#if __has_include("ValueAutomationPeer.g.cpp")
#include "ValueAutomationPeer.g.cpp"
#endif

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

ValueAutomationPeer::ValueAutomationPeer(xaml::UIElement const &uiElement) : m_uiElement(uiElement) {}

// IValueProvider
winrt::hstring ValueAutomationPeer::Value() {
  return GetAccessibilityValue(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue::Text);
}

// The following methods are not React Native props, but we need them to implement IValueProvider.
bool ValueAutomationPeer::IsReadOnly() {
  // We only support this for ComboBox now, which is interactive.
  return false;
}

void ValueAutomationPeer::SetValue(winrt::hstring const &value) {
  DynamicAutomationProperties::DispatchAccessibilityAction(m_uiElement, L"setValue");
}

// Private Methods
winrt::hstring ValueAutomationPeer::GetAccessibilityValue(
    winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue accValue) const {
  try {
    if (m_uiElement) {
      switch (accValue) {
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue::Text:
          return DynamicAutomationProperties::GetAccessibilityValueText(m_uiElement);
      }
    }
  } catch (...) {
  }

  return L"";
}

} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation
