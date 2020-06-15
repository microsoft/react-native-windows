// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationProperties.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

// Needed for latest versions of C++/WinRT
#if __has_include("DynamicAutomationProperties.g.cpp")
#include "DynamicAutomationProperties.g.cpp"
#endif

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Interop;
using namespace winrt::Windows::UI::Xaml::Interop;
} // namespace winrt

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

const winrt::TypeName dynamicAutomationTypeName{winrt::hstring{L"DynamicAutomationProperties"},
                                                winrt::TypeKind::Metadata};

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityRoleProperty() {
  static xaml::DependencyProperty s_AccessibilityRoleProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityRole",
      winrt::xaml_typename<winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Unknown)));

  return s_AccessibilityRoleProperty;
}

void DynamicAutomationProperties::SetAccessibilityRole(
    xaml::UIElement const &element,
    winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles const &value) {
  element.SetValue(AccessibilityRoleProperty(), winrt::box_value<PROJECT_ROOT_NAMESPACE::AccessibilityRoles>(value));
}

winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles DynamicAutomationProperties::GetAccessibilityRole(
    xaml::UIElement const &element) {
  return winrt::unbox_value<winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles>(
      element.GetValue(AccessibilityRoleProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityStateSelectedProperty() {
  static xaml::DependencyProperty s_AccessibilityStateSelectedProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityStateSelected",
      winrt::xaml_typename<bool>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateSelectedProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateSelected(xaml::UIElement const &element, bool value) {
  element.SetValue(AccessibilityStateSelectedProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateSelected(xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(element.GetValue(AccessibilityStateSelectedProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityStateDisabledProperty() {
  static xaml::DependencyProperty s_AccessibilityStateDisabledProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityStateDisabled",
      winrt::xaml_typename<bool>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateDisabledProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateDisabled(xaml::UIElement const &element, bool value) {
  element.SetValue(AccessibilityStateDisabledProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateDisabled(xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(element.GetValue(AccessibilityStateDisabledProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityStateCheckedProperty() {
  static xaml::DependencyProperty s_AccessibilityStateCheckedProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityStateChecked",
      winrt::xaml_typename<bool>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateCheckedProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateChecked(xaml::UIElement const &element, bool value) {
  element.SetValue(AccessibilityStateCheckedProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateChecked(xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(element.GetValue(AccessibilityStateCheckedProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityStateUncheckedProperty() {
  static xaml::DependencyProperty s_AccessibilityStateUncheckedProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityStateUnchecked",
      winrt::xaml_typename<bool>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateUncheckedProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateUnchecked(xaml::UIElement const &element, bool value) {
  element.SetValue(AccessibilityStateUncheckedProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateUnchecked(xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(element.GetValue(AccessibilityStateUncheckedProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityStateBusyProperty() {
  static xaml::DependencyProperty s_AccessibilityStateBusyProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityStateBusy",
      winrt::xaml_typename<bool>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateBusyProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateBusy(xaml::UIElement const &element, bool value) {
  element.SetValue(AccessibilityStateBusyProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateBusy(xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(element.GetValue(AccessibilityStateBusyProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityStateExpandedProperty() {
  static xaml::DependencyProperty s_AccessibilityStateExpandedProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityStateExpanded",
      winrt::xaml_typename<bool>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateExpandedProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateExpanded(xaml::UIElement const &element, bool value) {
  element.SetValue(AccessibilityStateExpandedProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateExpanded(xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(element.GetValue(AccessibilityStateExpandedProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityStateCollapsedProperty() {
  static xaml::DependencyProperty s_AccessibilityStateCollapsedProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityStateCollapsed",
      winrt::xaml_typename<bool>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(false)));

  return s_AccessibilityStateCollapsedProperty;
}

void DynamicAutomationProperties::SetAccessibilityStateCollapsed(xaml::UIElement const &element, bool value) {
  element.SetValue(AccessibilityStateCollapsedProperty(), winrt::box_value<bool>(value));
}

bool DynamicAutomationProperties::GetAccessibilityStateCollapsed(xaml::UIElement const &element) {
  return winrt::unbox_value<bool>(element.GetValue(AccessibilityStateCollapsedProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityValueMinProperty() {
  static xaml::DependencyProperty s_AccessibilityValueMinProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityValueMin",
      winrt::xaml_typename<double>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(0)));

  return s_AccessibilityValueMinProperty;
}

void DynamicAutomationProperties::SetAccessibilityValueMin(xaml::UIElement const &element, double value) {
  element.SetValue(AccessibilityValueMinProperty(), winrt::box_value<double>(value));
}

double DynamicAutomationProperties::GetAccessibilityValueMin(xaml::UIElement const &element) {
  return winrt::unbox_value<double>(element.GetValue(AccessibilityValueMinProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityValueMaxProperty() {
  static xaml::DependencyProperty s_AccessibilityValueMaxProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityValueMax",
      winrt::xaml_typename<double>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(0)));

  return s_AccessibilityValueMaxProperty;
}

void DynamicAutomationProperties::SetAccessibilityValueMax(xaml::UIElement const &element, double value) {
  element.SetValue(AccessibilityValueMaxProperty(), winrt::box_value<double>(value));
}

double DynamicAutomationProperties::GetAccessibilityValueMax(xaml::UIElement const &element) {
  return winrt::unbox_value<double>(element.GetValue(AccessibilityValueMaxProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityValueNowProperty() {
  static xaml::DependencyProperty s_AccessibilityValueNowProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityValueNow",
      winrt::xaml_typename<double>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(0)));

  return s_AccessibilityValueNowProperty;
}

void DynamicAutomationProperties::SetAccessibilityValueNow(xaml::UIElement const &element, double value) {
  element.SetValue(AccessibilityValueNowProperty(), winrt::box_value<double>(value));
}

double DynamicAutomationProperties::GetAccessibilityValueNow(xaml::UIElement const &element) {
  return winrt::unbox_value<double>(element.GetValue(AccessibilityValueNowProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityValueTextProperty() {
  static xaml::DependencyProperty s_AccessibilityValueTextProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityValueText",
      winrt::xaml_typename<winrt::hstring>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(winrt::box_value(L"")));

  return s_AccessibilityValueTextProperty;
}

void DynamicAutomationProperties::SetAccessibilityValueText(xaml::UIElement const &element, winrt::hstring value) {
  element.SetValue(AccessibilityValueTextProperty(), winrt::box_value(value));
}

winrt::hstring DynamicAutomationProperties::GetAccessibilityValueText(xaml::UIElement const &element) {
  return winrt::unbox_value<winrt::hstring>(element.GetValue(AccessibilityValueTextProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityInvokeEventHandlerProperty() {
  static xaml::DependencyProperty s_AccessibilityInvokeEventHandlerProperty =
      xaml::DependencyProperty::RegisterAttached(
          L"AccessibilityInvokeEventHandler",
          winrt::xaml_typename<winrt::PROJECT_ROOT_NAMESPACE::AccessibilityInvokeEventHandler>(),
          dynamicAutomationTypeName,
          winrt::PropertyMetadata(
              winrt::box_value<winrt::PROJECT_ROOT_NAMESPACE::AccessibilityInvokeEventHandler>(nullptr)));

  return s_AccessibilityInvokeEventHandlerProperty;
}

void DynamicAutomationProperties::SetAccessibilityInvokeEventHandler(
    xaml::UIElement const &element,
    winrt::PROJECT_ROOT_NAMESPACE::AccessibilityInvokeEventHandler const &value) {
  element.SetValue(
      AccessibilityInvokeEventHandlerProperty(),
      winrt::box_value<winrt::PROJECT_ROOT_NAMESPACE::AccessibilityInvokeEventHandler>(value));
}

winrt::PROJECT_ROOT_NAMESPACE::AccessibilityInvokeEventHandler
DynamicAutomationProperties::GetAccessibilityInvokeEventHandler(xaml::UIElement const &element) {
  return winrt::unbox_value<winrt::PROJECT_ROOT_NAMESPACE::AccessibilityInvokeEventHandler>(
      element.GetValue(AccessibilityInvokeEventHandlerProperty()));
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityActionsProperty() {
  static xaml::DependencyProperty s_AccessibilityActionsProperty = xaml::DependencyProperty::RegisterAttached(
      L"AccessibilityActions",
      winrt::xaml_typename<Windows::Foundation::Collections::IVector<PROJECT_ROOT_NAMESPACE::AccessibilityAction>>(),
      dynamicAutomationTypeName,
      winrt::PropertyMetadata(nullptr));

  return s_AccessibilityActionsProperty;
}

void DynamicAutomationProperties::SetAccessibilityActions(
    xaml::UIElement const &element,
    Windows::Foundation::Collections::IVector<PROJECT_ROOT_NAMESPACE::AccessibilityAction> const &value) {
  return element.SetValue(AccessibilityActionsProperty(), winrt::box_value(value));
}

Windows::Foundation::Collections::IVector<PROJECT_ROOT_NAMESPACE::AccessibilityAction>
DynamicAutomationProperties::GetAccessibilityActions(xaml::UIElement const &element) {
  return winrt::unbox_value<Windows::Foundation::Collections::IVector<PROJECT_ROOT_NAMESPACE::AccessibilityAction>>(
      element.GetValue(AccessibilityActionsProperty()));
}

void DynamicAutomationProperties::DispatchAccessibilityAction(
    xaml::UIElement const &element,
    std::wstring_view const &actionName) {
  if (element) {
    auto vector = GetAccessibilityActions(element);
    if (vector) {
      for (uint32_t i = 0; i < vector.Size(); i++) {
        auto item = vector.GetAt(i);

        if (item.Name.operator std::wstring_view() == actionName) {
          if (auto const &handler = GetAccessibilityActionEventHandler(element)) {
            handler(item);
          }
        }
      }
    }
  }
}

xaml::DependencyProperty DynamicAutomationProperties::AccessibilityActionEventHandlerProperty() {
  static xaml::DependencyProperty s_AccessibilityActionEventHandlerProperty =
      xaml::DependencyProperty::RegisterAttached(
          L"AccessibilityActionEventHandler",
          winrt::xaml_typename<winrt::PROJECT_ROOT_NAMESPACE::AccessibilityActionEventHandler>(),
          dynamicAutomationTypeName,
          winrt::PropertyMetadata(
              winrt::box_value<winrt::PROJECT_ROOT_NAMESPACE::AccessibilityActionEventHandler>(nullptr)));

  return s_AccessibilityActionEventHandlerProperty;
}

void DynamicAutomationProperties::SetAccessibilityActionEventHandler(
    xaml::UIElement const &element,
    winrt::PROJECT_ROOT_NAMESPACE::AccessibilityActionEventHandler const &value) {
  element.SetValue(
      AccessibilityActionEventHandlerProperty(),
      winrt::box_value<winrt::PROJECT_ROOT_NAMESPACE::AccessibilityActionEventHandler>(value));
}

winrt::PROJECT_ROOT_NAMESPACE::AccessibilityActionEventHandler
DynamicAutomationProperties::GetAccessibilityActionEventHandler(xaml::UIElement const &element) {
  return winrt::unbox_value<winrt::PROJECT_ROOT_NAMESPACE::AccessibilityActionEventHandler>(
      element.GetValue(AccessibilityActionEventHandlerProperty()));
}
} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation
