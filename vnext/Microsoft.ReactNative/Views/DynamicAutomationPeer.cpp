// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationPeer.h"
#include "DynamicAutomationProperties.h"
#include "DynamicValueProvider.h"

#include <UI.Xaml.Controls.h>

// Needed for latest versions of C++/WinRT
#if __has_include("DynamicAutomationPeer.g.cpp")
#include "DynamicAutomationPeer.g.cpp"
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

namespace winrt::Microsoft::ReactNative::implementation {

DynamicAutomationPeer::DynamicAutomationPeer(xaml::FrameworkElement const &owner) : Super(owner) {}

winrt::hstring DynamicAutomationPeer::GetClassNameCore() const {
  return L"DynamicAutomationPeer";
}

winrt::hstring DynamicAutomationPeer::GetNameCore() const {
  winrt::hstring name = Super::GetNameCore();

  if (name.empty()) {
    name = GetContentName();
  }

  return name;
}

winrt::AutomationControlType DynamicAutomationPeer::GetAutomationControlTypeCore() const {
  auto accessibilityRole = GetAccessibilityRole();

  switch (accessibilityRole) {
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Button:
    case winrt::Microsoft::ReactNative::AccessibilityRoles::ImageButton:
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Switch:
    case winrt::Microsoft::ReactNative::AccessibilityRoles::ToggleButton:
      return winrt::AutomationControlType::Button;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Link:
      return winrt::AutomationControlType::Hyperlink;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Image:
      return winrt::AutomationControlType::Image;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::KeyboardKey:
      return winrt::AutomationControlType::Custom;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Text:
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Header:
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Summary:
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Alert:
      return winrt::AutomationControlType::Text;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Adjustable:
      return winrt::AutomationControlType::Slider;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::CheckBox:
      return winrt::AutomationControlType::CheckBox;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::ComboBox:
      return winrt::AutomationControlType::ComboBox;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Menu:
      return winrt::AutomationControlType::Menu;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::MenuBar:
      return winrt::AutomationControlType::MenuBar;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::MenuItem:
      return winrt::AutomationControlType::MenuItem;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::ProgressBar:
      return winrt::AutomationControlType::ProgressBar;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Radio:
      return winrt::AutomationControlType::RadioButton;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::ScrollBar:
      return winrt::AutomationControlType::ScrollBar;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::SpinButton:
      return winrt::AutomationControlType::Spinner;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Tab:
      return winrt::AutomationControlType::TabItem;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::TabList:
      return winrt::AutomationControlType::Tab;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::ToolBar:
      return winrt::AutomationControlType::ToolBar;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::List:
      return winrt::AutomationControlType::List;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::ListItem:
      return winrt::AutomationControlType::ListItem;
    case winrt::Microsoft::ReactNative::AccessibilityRoles::None:
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Search:
    case winrt::Microsoft::ReactNative::AccessibilityRoles::RadioGroup:
    case winrt::Microsoft::ReactNative::AccessibilityRoles::Timer:
      return winrt::AutomationControlType::Group;
  }

  return Super::GetAutomationControlTypeCore();
}

winrt::IInspectable DynamicAutomationPeer::GetPatternCore(winrt::PatternInterface const &patternInterface) const {
  auto accessibilityRole = GetAccessibilityRole();

  if (patternInterface == winrt::PatternInterface::Invoke &&
      (accessibilityRole == winrt::Microsoft::ReactNative::AccessibilityRoles::Button ||
       accessibilityRole == winrt::Microsoft::ReactNative::AccessibilityRoles::ImageButton ||
       accessibilityRole == winrt::Microsoft::ReactNative::AccessibilityRoles::Radio)) {
    return *this;
  } else if (
      (patternInterface == winrt::PatternInterface::Selection ||
       patternInterface == winrt::PatternInterface::SelectionItem) &&
      HasAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Selected)) {
    return *this;
  } else if (
      patternInterface == winrt::PatternInterface::Toggle &&
      (accessibilityRole == winrt::Microsoft::ReactNative::AccessibilityRoles::CheckBox ||
       accessibilityRole == winrt::Microsoft::ReactNative::AccessibilityRoles::Switch ||
       accessibilityRole == winrt::Microsoft::ReactNative::AccessibilityRoles::Radio ||
       accessibilityRole == winrt::Microsoft::ReactNative::AccessibilityRoles::ToggleButton) &&
      (HasAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Checked) ||
       HasAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Unchecked))) {
    return *this;
  } else if (
      patternInterface == winrt::PatternInterface::ExpandCollapse &&
      (HasAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Expanded) ||
       HasAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Collapsed))) {
    return *this;
  } else if (
      patternInterface == winrt::PatternInterface::Value &&
      accessibilityRole != winrt::Microsoft::ReactNative::AccessibilityRoles::ProgressBar &&
      accessibilityRole != winrt::Microsoft::ReactNative::AccessibilityRoles::Adjustable &&
      accessibilityRole != winrt::Microsoft::ReactNative::AccessibilityRoles::ScrollBar &&
      (HasAccessibilityValue(winrt::Microsoft::ReactNative::AccessibilityValue::Text) ||
       HasAccessibilityValue(winrt::Microsoft::ReactNative::AccessibilityValue::Now))) {
    return winrt::make<winrt::Microsoft::ReactNative::implementation::DynamicValueProvider>(
        this->try_as<winrt::FrameworkElementAutomationPeer>());
  } else if (
      patternInterface == winrt::PatternInterface::RangeValue &&
      (accessibilityRole == winrt::Microsoft::ReactNative::AccessibilityRoles::ProgressBar ||
       accessibilityRole == winrt::Microsoft::ReactNative::AccessibilityRoles::Adjustable ||
       accessibilityRole == winrt::Microsoft::ReactNative::AccessibilityRoles::ScrollBar) &&
      (HasAccessibilityValue(winrt::Microsoft::ReactNative::AccessibilityValue::Now) &&
       HasAccessibilityValue(winrt::Microsoft::ReactNative::AccessibilityValue::Min) &&
       HasAccessibilityValue(winrt::Microsoft::ReactNative::AccessibilityValue::Max))) {
    return *this;
  }

  return Super::GetPatternCore(patternInterface);
}

bool DynamicAutomationPeer::IsEnabledCore() const {
  bool disabled = HasAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Disabled) &&
      GetAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Disabled);
  return !disabled && Super::IsEnabledCore();
}

winrt::hstring DynamicAutomationPeer::GetItemStatusCore() const {
  winrt::hstring itemStatus = Super::GetItemStatusCore();

  if (itemStatus.empty()) {
    if (HasAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Busy) &&
        GetAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Busy)) {
      itemStatus = L"Busy";
    }
  }

  return itemStatus;
}

// IInvokeProvider

void DynamicAutomationPeer::Invoke() const {
  DynamicAutomationProperties::DispatchAccessibilityAction(Owner(), L"invoke");

  if (auto const &invokeHandler = GetAccessibilityInvokeEventHandler()) {
    invokeHandler();
  }
}

// ISelectionProvider

winrt::com_array<winrt::IRawElementProviderSimple> DynamicAutomationPeer::GetSelection() const {
  // We don't differentiate between Views that are containers and Views that are
  // items, and we don't have a way to specify the linkage between the two

  // Returning nothing until that linkage exists
  return {};
}

// ISelectionItemProvider

bool DynamicAutomationPeer::IsSelected() const {
  return GetAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Selected);
}

winrt::IRawElementProviderSimple DynamicAutomationPeer::SelectionContainer() const {
  // We don't differentiate between Views that are containers and Views that are
  // items, and we don't have a way to specify the linkage between the two

  // Returning nothing until that linkage exists

  return nullptr;
}

void DynamicAutomationPeer::AddToSelection() const {
  DynamicAutomationProperties::DispatchAccessibilityAction(Owner(), L"addToSelection");
}

void DynamicAutomationPeer::RemoveFromSelection() const {
  DynamicAutomationProperties::DispatchAccessibilityAction(Owner(), L"removeFromSelection");
}

void DynamicAutomationPeer::Select() const {
  DynamicAutomationProperties::DispatchAccessibilityAction(Owner(), L"select");
}

// IToggleProvider

winrt::ToggleState DynamicAutomationPeer::ToggleState() const {
  bool checkedState = GetAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Checked);
  bool uncheckedState = GetAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Unchecked);

  if (!checkedState && uncheckedState) {
    return winrt::ToggleState::Off;
  } else if (checkedState && !uncheckedState) {
    return winrt::ToggleState::On;
  }

  return winrt::ToggleState::Indeterminate;
}

void DynamicAutomationPeer::Toggle() const {
  DynamicAutomationProperties::DispatchAccessibilityAction(Owner(), L"toggle");

  if (auto const &invokeHandler = GetAccessibilityInvokeEventHandler()) {
    invokeHandler();
  }
}

// IExpandCollapseProvider

winrt::ExpandCollapseState DynamicAutomationPeer::ExpandCollapseState() const {
  bool expandedState = GetAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Expanded);
  bool collapsedState = GetAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Collapsed);

  if (!expandedState && collapsedState) {
    return winrt::ExpandCollapseState::Collapsed;
  } else if (expandedState && !collapsedState) {
    return winrt::ExpandCollapseState::Expanded;
  } else if (expandedState && collapsedState) {
    return winrt::ExpandCollapseState::PartiallyExpanded;
  }

  return winrt::ExpandCollapseState::LeafNode;
}

void DynamicAutomationPeer::Expand() const {
  DynamicAutomationProperties::DispatchAccessibilityAction(Owner(), L"expand");
}

void DynamicAutomationPeer::Collapse() const {
  DynamicAutomationProperties::DispatchAccessibilityAction(Owner(), L"collapse");
}

// IRangeValueProvider
double DynamicAutomationPeer::Minimum() {
  return GetAccessibilityValueRange(winrt::Microsoft::ReactNative::AccessibilityValue::Min);
}

double DynamicAutomationPeer::Maximum() {
  return GetAccessibilityValueRange(winrt::Microsoft::ReactNative::AccessibilityValue::Max);
}

double DynamicAutomationPeer::Value() {
  return GetAccessibilityValueRange(winrt::Microsoft::ReactNative::AccessibilityValue::Now);
}

// Controls such as Slider, ProgressBar, ScrollBar are by definition interactive.
bool DynamicAutomationPeer::IsReadOnly() {
  return false;
}

void DynamicAutomationPeer::SetValue(double value) {
  try {
    DynamicAutomationProperties::DispatchAccessibilityAction(Owner(), L"setValue");
  } catch (...) {
  }
}

// Doesn't have a React Native analog.
// Return default value for XAML Slider.
// https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.primitives.rangebase.smallchange?view=winrt-19041
double DynamicAutomationPeer::SmallChange() {
  return 1;
}

// Doesn't have a React Native analog.
// Return default value for XAML Slider.
// https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.primitives.rangebase.largechange?view=winrt-19041
double DynamicAutomationPeer::LargeChange() {
  return 10;
}

// Private Methods
winrt::hstring DynamicAutomationPeer::GetContentName() const {
  winrt::hstring name = L"";

  try {
    if (auto const &viewControl = Owner().try_as<winrt::Microsoft::ReactNative::ViewControl>()) {
      auto viewPanel = viewControl.GetPanel();

      for (auto const &child : viewPanel.Children()) {
        if (auto const &textBlock = child.try_as<winrt::TextBlock>()) {
          name = name.empty() ? textBlock.Text() : (L" " + name + textBlock.Text());
        } else if (auto const &stringableName = child.try_as<winrt::IStringable>()) {
          name = (name.empty() ? L"" : L" ") + name + stringableName.ToString();
        }
      }
    }
  } catch (...) {
  }

  return name;
}

winrt::Microsoft::ReactNative::AccessibilityRoles DynamicAutomationPeer::GetAccessibilityRole() const {
  try {
    return DynamicAutomationProperties::GetAccessibilityRole(Owner());
  } catch (...) {
  }

  return winrt::Microsoft::ReactNative::AccessibilityRoles::None;
}

bool DynamicAutomationPeer::HasAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates state) const {
  try {
    auto const &owner = Owner();
    winrt::IInspectable value = nullptr;
    switch (state) {
      case winrt::Microsoft::ReactNative::AccessibilityStates::Selected:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateSelectedProperty());
        break;
      case winrt::Microsoft::ReactNative::AccessibilityStates::Disabled:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateDisabledProperty());
        break;
      case winrt::Microsoft::ReactNative::AccessibilityStates::Checked:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateCheckedProperty());
        break;
      case winrt::Microsoft::ReactNative::AccessibilityStates::Unchecked:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateUncheckedProperty());
        break;
      case winrt::Microsoft::ReactNative::AccessibilityStates::Busy:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateBusyProperty());
        break;
      case winrt::Microsoft::ReactNative::AccessibilityStates::Expanded:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateExpandedProperty());
        break;
      case winrt::Microsoft::ReactNative::AccessibilityStates::Collapsed:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateCollapsedProperty());
        break;
    }
    return (value != xaml::DependencyProperty::UnsetValue());
  } catch (...) {
  }

  return false;
}

bool DynamicAutomationPeer::GetAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates state) const {
  try {
    auto const &owner = Owner();
    switch (state) {
      case winrt::Microsoft::ReactNative::AccessibilityStates::Selected:
        return DynamicAutomationProperties::GetAccessibilityStateSelected(owner);
      case winrt::Microsoft::ReactNative::AccessibilityStates::Disabled:
        return DynamicAutomationProperties::GetAccessibilityStateDisabled(owner);
      case winrt::Microsoft::ReactNative::AccessibilityStates::Checked:
        return DynamicAutomationProperties::GetAccessibilityStateChecked(owner);
      case winrt::Microsoft::ReactNative::AccessibilityStates::Unchecked:
        return DynamicAutomationProperties::GetAccessibilityStateUnchecked(owner);
      case winrt::Microsoft::ReactNative::AccessibilityStates::Busy:
        return DynamicAutomationProperties::GetAccessibilityStateBusy(owner);
      case winrt::Microsoft::ReactNative::AccessibilityStates::Expanded:
        return DynamicAutomationProperties::GetAccessibilityStateExpanded(owner);
      case winrt::Microsoft::ReactNative::AccessibilityStates::Collapsed:
        return DynamicAutomationProperties::GetAccessibilityStateCollapsed(owner);
    }
  } catch (...) {
  }

  return false;
}

bool DynamicAutomationPeer::HasAccessibilityValue(winrt::Microsoft::ReactNative::AccessibilityValue accValue) const {
  try {
    auto const &owner = Owner();
    winrt::IInspectable value = nullptr;

    switch (accValue) {
      case winrt::Microsoft::ReactNative::AccessibilityValue::Min:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityValueMinProperty());
        break;
      case winrt::Microsoft::ReactNative::AccessibilityValue::Max:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityValueMaxProperty());
        break;
      case winrt::Microsoft::ReactNative::AccessibilityValue::Now:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityValueNowProperty());
        break;
      case winrt::Microsoft::ReactNative::AccessibilityValue::Text:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityValueTextProperty());
        break;
    }
    return (value != xaml::DependencyProperty::UnsetValue());
  } catch (...) {
  }

  return false;
}

winrt::Microsoft::ReactNative::AccessibilityInvokeEventHandler
DynamicAutomationPeer::GetAccessibilityInvokeEventHandler() const {
  try {
    return DynamicAutomationProperties::GetAccessibilityInvokeEventHandler(Owner());
  } catch (...) {
  }

  return nullptr;
}

double DynamicAutomationPeer::GetAccessibilityValueRange(
    winrt::Microsoft::ReactNative::AccessibilityValue accValue) const {
  try {
    auto const &owner = Owner();

    switch (accValue) {
      case winrt::Microsoft::ReactNative::AccessibilityValue::Min:
        return DynamicAutomationProperties::GetAccessibilityValueMin(owner);
      case winrt::Microsoft::ReactNative::AccessibilityValue::Max:
        return DynamicAutomationProperties::GetAccessibilityValueMax(owner);
      case winrt::Microsoft::ReactNative::AccessibilityValue::Now:
        return DynamicAutomationProperties::GetAccessibilityValueNow(owner);
    }
  } catch (...) {
  }

  return 0;
}

} // namespace winrt::Microsoft::ReactNative::implementation
