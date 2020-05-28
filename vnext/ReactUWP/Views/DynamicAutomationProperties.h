// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "DynamicAutomationProperties.g.h"

#ifndef PROJECT_ROOT_NAMESPACE
#define PROJECT_ROOT_NAMESPACE react::uwp
#else
namespace winrt::Microsoft::ReactNative {}
namespace winrt::react::uwp {
using namespace winrt::Microsoft::ReactNative;
}
#endif

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

//
// DynamicAutomationProperties provides attached properties for the various
// accessibility related properties in RN that will be used by
// DynamicAutomationPeer
//
struct DynamicAutomationProperties : DynamicAutomationPropertiesT<DynamicAutomationProperties> {
  using Super = DynamicAutomationPropertiesT<DynamicAutomationProperties>;

 public:
  DynamicAutomationProperties() = delete;

  // Attached Properties

  static xaml::DependencyProperty AccessibilityRoleProperty();
  static void SetAccessibilityRole(
      xaml::UIElement const &element,
      winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles const &value);
  static AccessibilityRoles GetAccessibilityRole(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityStateSelectedProperty();
  static void SetAccessibilityStateSelected(xaml::UIElement const &element, bool value);
  static bool GetAccessibilityStateSelected(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityStateDisabledProperty();
  static void SetAccessibilityStateDisabled(xaml::UIElement const &element, bool value);
  static bool GetAccessibilityStateDisabled(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityStateCheckedProperty();
  static void SetAccessibilityStateChecked(xaml::UIElement const &element, bool value);
  static bool GetAccessibilityStateChecked(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityStateUncheckedProperty();
  static void SetAccessibilityStateUnchecked(xaml::UIElement const &element, bool value);
  static bool GetAccessibilityStateUnchecked(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityStateBusyProperty();
  static void SetAccessibilityStateBusy(xaml::UIElement const &element, bool value);
  static bool GetAccessibilityStateBusy(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityStateExpandedProperty();
  static void SetAccessibilityStateExpanded(xaml::UIElement const &element, bool value);
  static bool GetAccessibilityStateExpanded(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityStateCollapsedProperty();
  static void SetAccessibilityStateCollapsed(xaml::UIElement const &element, bool value);
  static bool GetAccessibilityStateCollapsed(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityValueMinProperty();
  static void SetAccessibilityValueMin(xaml::UIElement const &element, double value);
  static double GetAccessibilityValueMin(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityValueMaxProperty();
  static void SetAccessibilityValueMax(xaml::UIElement const &element, double value);
  static double GetAccessibilityValueMax(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityValueNowProperty();
  static void SetAccessibilityValueNow(xaml::UIElement const &element, double value);
  static double GetAccessibilityValueNow(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityValueTextProperty();
  static void SetAccessibilityValueText(xaml::UIElement const &element, winrt::hstring value);
  static winrt::hstring GetAccessibilityValueText(xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityInvokeEventHandlerProperty();
  static void SetAccessibilityInvokeEventHandler(
      xaml::UIElement const &element,
      winrt::PROJECT_ROOT_NAMESPACE::AccessibilityInvokeEventHandler const &value);
  static winrt::PROJECT_ROOT_NAMESPACE::AccessibilityInvokeEventHandler GetAccessibilityInvokeEventHandler(
      xaml::UIElement const &element);

  static xaml::DependencyProperty AccessibilityActionsProperty();

  static void SetAccessibilityActions(
      xaml::UIElement const &element,
      Windows::Foundation::Collections::IVector<PROJECT_ROOT_NAMESPACE::AccessibilityAction> const &value);

  static Windows::Foundation::Collections::IVector<PROJECT_ROOT_NAMESPACE::AccessibilityAction> GetAccessibilityActions(
      xaml::UIElement const &element);

  static void DispatchAccessibilityAction(xaml::UIElement const &element, std::wstring_view const &actionName);

  static xaml::DependencyProperty AccessibilityActionEventHandlerProperty();
  static void SetAccessibilityActionEventHandler(
      xaml::UIElement const &element,
      winrt::PROJECT_ROOT_NAMESPACE::AccessibilityActionEventHandler const &value);
  static winrt::PROJECT_ROOT_NAMESPACE::AccessibilityActionEventHandler GetAccessibilityActionEventHandler(
      xaml::UIElement const &element);
};

} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation

namespace winrt::PROJECT_ROOT_NAMESPACE::factory_implementation {
struct DynamicAutomationProperties
    : DynamicAutomationPropertiesT<DynamicAutomationProperties, implementation::DynamicAutomationProperties> {};
} // namespace winrt::PROJECT_ROOT_NAMESPACE::factory_implementation

namespace react::uwp {
// Issue #2172: Calling static members on
// winrt::react::uwp::DynamicAutomationProperties fails to call
// down into winrt::react::uwp::implementation::DynamicAutomationProperties
// because of how we're
// using cppwinrt. This workaround is so that consumers in react::uwp can just
// call DynamicAutomationProperties

using DynamicAutomationProperties = winrt::PROJECT_ROOT_NAMESPACE::implementation::DynamicAutomationProperties;
} // namespace react::uwp
