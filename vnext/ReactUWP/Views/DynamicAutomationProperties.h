// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.UI.Xaml.h>

#include "cppwinrt/DynamicAutomationProperties.g.h"

namespace winrt::react::uwp::implementation {

//
// DynamicAutomationProperties provides attached properties for the various
// accessibility related properties in RN that will be used by
// DynamicAutomationPeer
//
struct DynamicAutomationProperties
    : DynamicAutomationPropertiesT<DynamicAutomationProperties> {
  using Super = DynamicAutomationPropertiesT<DynamicAutomationProperties>;

 public:
  DynamicAutomationProperties() = delete;

  // Attached Properties

  static winrt::Windows::UI::Xaml::DependencyProperty
  AccessibilityRoleProperty();
  static void SetAccessibilityRole(
      winrt::Windows::UI::Xaml::UIElement const &element,
      winrt::react::uwp::AccessibilityRoles const &value);
  static AccessibilityRoles GetAccessibilityRole(
      winrt::Windows::UI::Xaml::UIElement const &element);

  static winrt::Windows::UI::Xaml::DependencyProperty
  AccessibilityStateSelectedProperty();
  static void SetAccessibilityStateSelected(
      winrt::Windows::UI::Xaml::UIElement const &element,
      bool value);
  static bool GetAccessibilityStateSelected(
      winrt::Windows::UI::Xaml::UIElement const &element);

  static winrt::Windows::UI::Xaml::DependencyProperty
  AccessibilityStateDisabledProperty();
  static void SetAccessibilityStateDisabled(
      winrt::Windows::UI::Xaml::UIElement const &element,
      bool value);
  static bool GetAccessibilityStateDisabled(
      winrt::Windows::UI::Xaml::UIElement const &element);

  static winrt::Windows::UI::Xaml::DependencyProperty
  AccessibilityStateCheckedProperty();
  static void SetAccessibilityStateChecked(
      winrt::Windows::UI::Xaml::UIElement const &element,
      bool value);
  static bool GetAccessibilityStateChecked(
      winrt::Windows::UI::Xaml::UIElement const &element);

  static winrt::Windows::UI::Xaml::DependencyProperty
  AccessibilityStateUncheckedProperty();
  static void SetAccessibilityStateUnchecked(
      winrt::Windows::UI::Xaml::UIElement const &element,
      bool value);
  static bool GetAccessibilityStateUnchecked(
      winrt::Windows::UI::Xaml::UIElement const &element);

  static winrt::Windows::UI::Xaml::DependencyProperty
  AccessibilityStateBusyProperty();
  static void SetAccessibilityStateBusy(
      winrt::Windows::UI::Xaml::UIElement const &element,
      bool value);
  static bool GetAccessibilityStateBusy(
      winrt::Windows::UI::Xaml::UIElement const &element);

  static winrt::Windows::UI::Xaml::DependencyProperty
  AccessibilityStateExpandedProperty();
  static void SetAccessibilityStateExpanded(
      winrt::Windows::UI::Xaml::UIElement const &element,
      bool value);
  static bool GetAccessibilityStateExpanded(
      winrt::Windows::UI::Xaml::UIElement const &element);

  static winrt::Windows::UI::Xaml::DependencyProperty
  AccessibilityStateCollapsedProperty();
  static void SetAccessibilityStateCollapsed(
      winrt::Windows::UI::Xaml::UIElement const &element,
      bool value);
  static bool GetAccessibilityStateCollapsed(
      winrt::Windows::UI::Xaml::UIElement const &element);

  static winrt::Windows::UI::Xaml::DependencyProperty
  AccessibilityInvokeEventHandlerProperty();
  static void SetAccessibilityInvokeEventHandler(
      Windows::UI::Xaml::UIElement const &element,
      winrt::react::uwp::AccessibilityInvokeEventHandler const &value);
  static winrt::react::uwp::AccessibilityInvokeEventHandler
  GetAccessibilityInvokeEventHandler(
      winrt::Windows::UI::Xaml::UIElement const &element);
};

} // namespace winrt::react::uwp::implementation

namespace winrt::react::uwp::factory_implementation {
struct DynamicAutomationProperties
    : DynamicAutomationPropertiesT<
          DynamicAutomationProperties,
          implementation::DynamicAutomationProperties> {};
} // namespace winrt::react::uwp::factory_implementation

namespace react::uwp {
// Issue #2172: Calling static members on
// winrt::react::uwp::DynamicAutomationProperties fails to call
// down into winrt::react::uwp::implementation::DynamicAutomationProperties
// because of how we're
// using cppwinrt. This workaround is so that consumers in react::uwp can just
// call DynamicAutomationProperties

using DynamicAutomationProperties =
    winrt::react::uwp::implementation::DynamicAutomationProperties;
} // namespace react::uwp
