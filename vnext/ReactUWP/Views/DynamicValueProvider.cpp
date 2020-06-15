// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationProperties.h"
#include "DynamicValueProvider.h"

#include <UI.Xaml.Controls.h>

// Needed for latest versions of C++/WinRT
#if __has_include("DynamicValueProvider.g.cpp")
#include "DynamicValueProvider.g.cpp"
#endif

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

DynamicValueProvider::DynamicValueProvider(xaml::Automation::Peers::FrameworkElementAutomationPeer peer)
    : m_peer(peer) {}

// IValueProvider
winrt::hstring DynamicValueProvider::Value() {
  return GetAccessibilityValue(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue::Text);
}

// The following methods are not React Native props, but we need them to implement IValueProvider.
bool DynamicValueProvider::IsReadOnly() {
  return false;
}

void DynamicValueProvider::SetValue(winrt::hstring const &value) {
  try {
    if (auto const &owner = m_peer.Owner()) {
      DynamicAutomationProperties::DispatchAccessibilityAction(owner, L"setValue");
    }
  } catch (...) {
  }
}

// Private Methods
winrt::hstring DynamicValueProvider::GetAccessibilityValue(
    winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue accValue) const {
  try {
    if (auto const &owner = m_peer.Owner()) {
      switch (accValue) {
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityValue::Text:
          return DynamicAutomationProperties::GetAccessibilityValueText(owner);
      }
    }
  } catch (...) {
  }

  return L"";
}

} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation
