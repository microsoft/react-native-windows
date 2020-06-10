// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "RangeAutomationPeer.h"
#include "DynamicAutomationProperties.h"

#include <UI.Xaml.Controls.h>

// Needed for latest versions of C++/WinRT
#if __has_include("RangeAutomationPeer.g.cpp")
#include "RangeAutomationPeer.g.cpp"
#endif

namespace winrt {
using namespace Windows::Foundation;
using namespace xaml;
using namespace xaml::Automation;
using namespace xaml::Automation::Peers;
using namespace xaml::Automation::Provider;
using namespace xaml::Controls;
using namespace xaml::Interop;
using namespace xaml::Media;
} // namespace winrt

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

// TODO
// RN doesn't implement this
double RangeAutomationPeer::SmallChange() {
  return 0;
}

// TODO
// RN doesn't implement this
double RangeAutomationPeer::LargeChange() {
  return 0;
}

// TODO 
// RN doesn't implement this
bool RangeAutomationPeer::IsReadOnly() {
  return false;
}

void RangeAutomationPeer::SetValue(double value) {
  DynamicAutomationProperties::DispatchAccessibilityAction(m_uiElement, L"setValue");
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
