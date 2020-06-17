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
  return GetAccessibilityValue();
}

// The following methods are not React Native props, but we need them to implement IValueProvider.
bool DynamicValueProvider::IsReadOnly() {
  return false;
}

void DynamicValueProvider::SetValue(winrt::hstring const &value) {
  try {
    DynamicAutomationProperties::DispatchAccessibilityAction(m_peer.Owner(), L"setValue");
  } catch (...) {
  }
}

// Private Methods
winrt::hstring DynamicValueProvider::GetAccessibilityValue() const {
  try {
    winrt::IInspectable value = nullptr;
    auto const &owner = m_peer.Owner();

    value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityValueTextProperty());
    if (value != xaml::DependencyProperty::UnsetValue()) {
      return DynamicAutomationProperties::GetAccessibilityValueText(owner);
    } else {
      value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityValueNowProperty());
      if (value != xaml::DependencyProperty::UnsetValue()) {
        return std::to_wstring(DynamicAutomationProperties::GetAccessibilityValueNow(owner)).c_str();
      }
    }
  } catch (...) {
  }

  return L"";
}

} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation
