// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationProperties.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Interop;
} // namespace winrt

namespace winrt::react::uwp::implementation {

const winrt::TypeName dynamicAutomationTypeName{
    winrt::hstring{L"DynamicAutomationProperties"},
    winrt::TypeKind::Metadata};

winrt::Windows::UI::Xaml::DependencyProperty
DynamicAutomationProperties::AccessibilityRoleProperty() {
  static winrt::DependencyProperty s_AccessibilityRoleProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"AccessibilityRole",
          winrt::xaml_typename<winrt::react::uwp::AccessibilityRoles>(),
          dynamicAutomationTypeName,
          winrt::PropertyMetadata(winrt::box_value(
              winrt::react::uwp::AccessibilityRoles::Unknown)));

  return s_AccessibilityRoleProperty;
}

void DynamicAutomationProperties::SetAccessibilityRole(
    winrt::Windows::UI::Xaml::UIElement const &element,
    winrt::react::uwp::AccessibilityRoles const &value) {
  element.SetValue(
      AccessibilityRoleProperty(),
      winrt::box_value<react::uwp::AccessibilityRoles>(value));
}

winrt::react::uwp::AccessibilityRoles
DynamicAutomationProperties::GetAccessibilityRole(
    winrt::Windows::UI::Xaml::UIElement const &element) {
  return winrt::unbox_value<winrt::react::uwp::AccessibilityRoles>(
      element.GetValue(AccessibilityRoleProperty()));
}

winrt::Windows::UI::Xaml::DependencyProperty
DynamicAutomationProperties::AccessibilityStateSelectedProperty() {
  static winrt::DependencyProperty s_AccessibilityStateSelectedProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"AccessibilityStateSelected",
          winrt::xaml_typename<bool>(),
          dynamicAutomationTypeName,
          winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateSelectedProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateSelected(
    winrt::Windows::UI::Xaml::UIElement const &element,
    bool value) {
  element.SetValue(
      AccessibilityStateSelectedProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateSelected(
    winrt::Windows::UI::Xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(
      element.GetValue(AccessibilityStateSelectedProperty()));
}

winrt::Windows::UI::Xaml::DependencyProperty
DynamicAutomationProperties::AccessibilityStateDisabledProperty() {
  static winrt::DependencyProperty s_AccessibilityStateDisabledProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"AccessibilityStateDisabled",
          winrt::xaml_typename<bool>(),
          dynamicAutomationTypeName,
          winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateDisabledProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateDisabled(
    winrt::Windows::UI::Xaml::UIElement const &element,
    bool value) {
  element.SetValue(
      AccessibilityStateDisabledProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateDisabled(
    winrt::Windows::UI::Xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(
      element.GetValue(AccessibilityStateDisabledProperty()));
}

winrt::Windows::UI::Xaml::DependencyProperty
DynamicAutomationProperties::AccessibilityStateCheckedProperty() {
  static winrt::DependencyProperty s_AccessibilityStateCheckedProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"AccessibilityStateChecked",
          winrt::xaml_typename<bool>(),
          dynamicAutomationTypeName,
          winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateCheckedProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateChecked(
    winrt::Windows::UI::Xaml::UIElement const &element,
    bool value) {
  element.SetValue(
      AccessibilityStateCheckedProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateChecked(
    winrt::Windows::UI::Xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(
      element.GetValue(AccessibilityStateCheckedProperty()));
}

winrt::Windows::UI::Xaml::DependencyProperty
DynamicAutomationProperties::AccessibilityStateUncheckedProperty() {
  static winrt::DependencyProperty s_AccessibilityStateUncheckedProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"AccessibilityStateUnchecked",
          winrt::xaml_typename<bool>(),
          dynamicAutomationTypeName,
          winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateUncheckedProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateUnchecked(
    winrt::Windows::UI::Xaml::UIElement const &element,
    bool value) {
  element.SetValue(
      AccessibilityStateUncheckedProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateUnchecked(
    winrt::Windows::UI::Xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(
      element.GetValue(AccessibilityStateUncheckedProperty()));
}

winrt::Windows::UI::Xaml::DependencyProperty
DynamicAutomationProperties::AccessibilityStateBusyProperty() {
  static winrt::DependencyProperty s_AccessibilityStateBusyProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"AccessibilityStateBusy",
          winrt::xaml_typename<bool>(),
          dynamicAutomationTypeName,
          winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateBusyProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateBusy(
    winrt::Windows::UI::Xaml::UIElement const &element,
    bool value) {
  element.SetValue(
      AccessibilityStateBusyProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateBusy(
    winrt::Windows::UI::Xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(
      element.GetValue(AccessibilityStateBusyProperty()));
}

winrt::Windows::UI::Xaml::DependencyProperty
DynamicAutomationProperties::AccessibilityStateExpandedProperty() {
  static winrt::DependencyProperty s_AccessibilityStateExpandedProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"AccessibilityStateExpanded",
          winrt::xaml_typename<bool>(),
          dynamicAutomationTypeName,
          winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateExpandedProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateExpanded(
    winrt::Windows::UI::Xaml::UIElement const &element,
    bool value) {
  element.SetValue(
      AccessibilityStateExpandedProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateExpanded(
    winrt::Windows::UI::Xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(
      element.GetValue(AccessibilityStateExpandedProperty()));
}

winrt::Windows::UI::Xaml::DependencyProperty
DynamicAutomationProperties::AccessibilityStateCollapsedProperty() {
  static winrt::DependencyProperty s_AccessibilityStateCollapsedProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"AccessibilityStateCollapsed",
          winrt::xaml_typename<bool>(),
          dynamicAutomationTypeName,
          winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateCollapsedProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateCollapsed(
    winrt::Windows::UI::Xaml::UIElement const &element,
    bool value) {
  element.SetValue(
      AccessibilityStateCollapsedProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateCollapsed(
    winrt::Windows::UI::Xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(
      element.GetValue(AccessibilityStateCollapsedProperty()));
}

winrt::Windows::UI::Xaml::DependencyProperty
DynamicAutomationProperties::AccessibilityInvokeEventHandlerProperty() {
  static winrt::DependencyProperty s_AccessibilityInvokeEventHandlerProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"AccessibilityInvokeEventHandler",
          winrt::xaml_typename<
              winrt::react::uwp::AccessibilityInvokeEventHandler>(),
          dynamicAutomationTypeName,
          winrt::PropertyMetadata(
              winrt::box_value<
                  winrt::react::uwp::AccessibilityInvokeEventHandler>(
                  nullptr)));

  return s_AccessibilityInvokeEventHandlerProperty;
}

void DynamicAutomationProperties::SetAccessibilityInvokeEventHandler(
    winrt::Windows::UI::Xaml::UIElement const &element,
    winrt::react::uwp::AccessibilityInvokeEventHandler const &value) {
  element.SetValue(
      AccessibilityInvokeEventHandlerProperty(),
      winrt::box_value<winrt::react::uwp::AccessibilityInvokeEventHandler>(
          value));
}

winrt::react::uwp::AccessibilityInvokeEventHandler
DynamicAutomationProperties::GetAccessibilityInvokeEventHandler(
    winrt::Windows::UI::Xaml::UIElement const &element) {
  return winrt::unbox_value<winrt::react::uwp::AccessibilityInvokeEventHandler>(
      element.GetValue(AccessibilityInvokeEventHandlerProperty()));
}

} // namespace winrt::react::uwp::implementation
