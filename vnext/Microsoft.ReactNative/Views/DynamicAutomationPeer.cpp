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
  const auto automationControlType = GetAutomationControlTypeFromAriaRole();
  return automationControlType ? automationControlType.value() : GetAutomationControlTypeFromAccessibilityRole();
}

std::optional<winrt::AutomationControlType> DynamicAutomationPeer::GetAutomationControlTypeFromAriaRole() const {
  const auto ariaRole = GetAriaRole();
  // Sourced from: https://www.w3.org/TR/core-aam-1.2
  // Remaining unmapped roles are:
  // - "none"
  // - "presentation"
  switch (ariaRole) {
    case winrt::Microsoft::ReactNative::AriaRole::Alert:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::AlertDialog:
      return winrt::AutomationControlType::Pane;
    case winrt::Microsoft::ReactNative::AriaRole::Application:
      return winrt::AutomationControlType::Pane;
    case winrt::Microsoft::ReactNative::AriaRole::Article:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Banner:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Button:
      return winrt::AutomationControlType::Button;
    case winrt::Microsoft::ReactNative::AriaRole::Cell:
      return winrt::AutomationControlType::DataItem;
    case winrt::Microsoft::ReactNative::AriaRole::CheckBox:
      return winrt::AutomationControlType::CheckBox;
    case winrt::Microsoft::ReactNative::AriaRole::ColumnHeader:
      return winrt::AutomationControlType::DataItem;
    case winrt::Microsoft::ReactNative::AriaRole::ComboBox:
      return winrt::AutomationControlType::ComboBox;
    case winrt::Microsoft::ReactNative::AriaRole::Complementary:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::ContentInfo:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Definition:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Dialog:
      return winrt::AutomationControlType::Pane;
    case winrt::Microsoft::ReactNative::AriaRole::Directory:
      return winrt::AutomationControlType::List;
    case winrt::Microsoft::ReactNative::AriaRole::Document:
      return winrt::AutomationControlType::Document;
    case winrt::Microsoft::ReactNative::AriaRole::Feed:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Figure:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Form:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Grid:
      return winrt::AutomationControlType::DataGrid;
    case winrt::Microsoft::ReactNative::AriaRole::Group:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Heading:
      return winrt::AutomationControlType::Text;
    case winrt::Microsoft::ReactNative::AriaRole::Img:
      return winrt::AutomationControlType::Image;
    case winrt::Microsoft::ReactNative::AriaRole::Link:
      return winrt::AutomationControlType::Hyperlink;
    case winrt::Microsoft::ReactNative::AriaRole::List:
      return winrt::AutomationControlType::List;
    case winrt::Microsoft::ReactNative::AriaRole::ListItem:
      return winrt::AutomationControlType::ListItem;
    case winrt::Microsoft::ReactNative::AriaRole::Log:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Main:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Marquee:
      return winrt::AutomationControlType::Text;
    case winrt::Microsoft::ReactNative::AriaRole::Math:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Menu:
      return winrt::AutomationControlType::Menu;
    case winrt::Microsoft::ReactNative::AriaRole::MenuBar:
      return winrt::AutomationControlType::MenuBar;
    case winrt::Microsoft::ReactNative::AriaRole::MenuItem:
      return winrt::AutomationControlType::MenuItem;
    case winrt::Microsoft::ReactNative::AriaRole::Meter:
      return winrt::AutomationControlType::ProgressBar;
    case winrt::Microsoft::ReactNative::AriaRole::Navigation:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Note:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Option:
      return winrt::AutomationControlType::ListItem;
    case winrt::Microsoft::ReactNative::AriaRole::ProgressBar:
      return winrt::AutomationControlType::ProgressBar;
    case winrt::Microsoft::ReactNative::AriaRole::Radio:
      return winrt::AutomationControlType::RadioButton;
    case winrt::Microsoft::ReactNative::AriaRole::RadioGroup:
      return winrt::AutomationControlType::List;
    case winrt::Microsoft::ReactNative::AriaRole::Region:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Row:
      return winrt::AutomationControlType::DataItem;
    case winrt::Microsoft::ReactNative::AriaRole::RowGroup:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::RowHeader:
      return winrt::AutomationControlType::HeaderItem;
    case winrt::Microsoft::ReactNative::AriaRole::ScrollBar:
      return winrt::AutomationControlType::ScrollBar;
    case winrt::Microsoft::ReactNative::AriaRole::SearchBox:
      return winrt::AutomationControlType::Edit;
    case winrt::Microsoft::ReactNative::AriaRole::Separator:
      if (auto const &viewControl = Owner().try_as<winrt::Microsoft::ReactNative::ViewControl>()) {
        if (viewControl.IsTabStop()) {
          return winrt::AutomationControlType::Thumb;
        }
      }
      return winrt::AutomationControlType::Separator;
    case winrt::Microsoft::ReactNative::AriaRole::Slider:
      return winrt::AutomationControlType::Slider;
    case winrt::Microsoft::ReactNative::AriaRole::SpinButton:
      return winrt::AutomationControlType::Spinner;
    case winrt::Microsoft::ReactNative::AriaRole::Status:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::Switch:
      return winrt::AutomationControlType::Button;
    case winrt::Microsoft::ReactNative::AriaRole::Tab:
      return winrt::AutomationControlType::TabItem;
    case winrt::Microsoft::ReactNative::AriaRole::Table:
      return winrt::AutomationControlType::DataGrid;
    case winrt::Microsoft::ReactNative::AriaRole::TabList:
      return winrt::AutomationControlType::Tab;
    case winrt::Microsoft::ReactNative::AriaRole::TabPanel:
      return winrt::AutomationControlType::Pane;
    case winrt::Microsoft::ReactNative::AriaRole::Term:
      return winrt::AutomationControlType::Text;
    case winrt::Microsoft::ReactNative::AriaRole::Timer:
      return winrt::AutomationControlType::Group;
    case winrt::Microsoft::ReactNative::AriaRole::ToolBar:
      return winrt::AutomationControlType::ToolBar;
    case winrt::Microsoft::ReactNative::AriaRole::ToolTip:
      return winrt::AutomationControlType::ToolTip;
    case winrt::Microsoft::ReactNative::AriaRole::Tree:
      return winrt::AutomationControlType::Tree;
    case winrt::Microsoft::ReactNative::AriaRole::TreeGrid:
      return winrt::AutomationControlType::DataGrid;
    case winrt::Microsoft::ReactNative::AriaRole::TreeItem:
      return winrt::AutomationControlType::TreeItem;
    case winrt::Microsoft::ReactNative::AriaRole::None:
    case winrt::Microsoft::ReactNative::AriaRole::Presentation:
    default:
      return std::nullopt;
  }
}

winrt::AutomationControlType DynamicAutomationPeer::GetAutomationControlTypeFromAccessibilityRole() const {
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
      HasAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Checked)) {
    return *this;
  } else if (
      patternInterface == winrt::PatternInterface::ExpandCollapse &&
      HasAccessibilityState(winrt::Microsoft::ReactNative::AccessibilityStates::Expanded)) {
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
  bool disabled = false;
  try {
    disabled = DynamicAutomationProperties::GetAccessibilityStateDisabled(Owner());
  } catch (...) {
  }
  return !disabled && Super::IsEnabledCore();
}

winrt::hstring DynamicAutomationPeer::GetItemStatusCore() const {
  winrt::hstring itemStatus = Super::GetItemStatusCore();

  if (itemStatus.empty()) {
    try {
      if (DynamicAutomationProperties::GetAccessibilityStateBusy(Owner())) {
        itemStatus = L"Busy";
      }
    } catch (...) {
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
  bool selected = false;
  try {
    selected = DynamicAutomationProperties::GetAccessibilityStateSelected(Owner());
  } catch (...) {
  }
  return selected;
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
  auto checkedState = winrt::ToggleState::Off;

  try {
    checkedState = static_cast<winrt::ToggleState>(DynamicAutomationProperties::GetAccessibilityStateChecked(Owner()));
  } catch (...) {
  }

  return checkedState;
}

void DynamicAutomationPeer::Toggle() const {
  DynamicAutomationProperties::DispatchAccessibilityAction(Owner(), L"toggle");

  if (auto const &invokeHandler = GetAccessibilityInvokeEventHandler()) {
    invokeHandler();
  }
}

// IExpandCollapseProvider

winrt::ExpandCollapseState DynamicAutomationPeer::ExpandCollapseState() const {
  auto expandedState = winrt::ExpandCollapseState::Collapsed;

  try {
    expandedState = DynamicAutomationProperties::GetAccessibilityStateExpanded(Owner())
        ? winrt::ExpandCollapseState::Expanded
        : winrt::ExpandCollapseState::Collapsed;
  } catch (...) {
  }
  return expandedState;
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
// https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.primitives.rangebase.smallchange?view=winrt-22621
double DynamicAutomationPeer::SmallChange() {
  return 1;
}

// Doesn't have a React Native analog.
// Return default value for XAML Slider.
// https://docs.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.primitives.rangebase.largechange?view=winrt-22621
double DynamicAutomationPeer::LargeChange() {
  return 10;
}

// Private Methods
winrt::hstring DynamicAutomationPeer::GetContentName() const {
  winrt::hstring name = L"";

  try {
    if (auto const &viewControl = Owner().try_as<winrt::Microsoft::ReactNative::ViewControl>()) {
      // It's possible that the ViewPanel is null, perhaps when this is invoked around the same time
      // that a View is being unmounted, causing a fast fail for EXCEPTION_ACCESS_VIOLATION_READ.
      if (auto viewPanel = viewControl.GetPanel()) {
        for (auto const &child : viewPanel.Children()) {
          if (auto const &textBlock = child.try_as<winrt::TextBlock>()) {
            name = name.empty() ? textBlock.Text() : (L" " + name + textBlock.Text());
          } else if (auto const &stringableName = child.try_as<winrt::IStringable>()) {
            name = (name.empty() ? L"" : L" ") + name + stringableName.ToString();
          }
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

winrt::Microsoft::ReactNative::AriaRole DynamicAutomationPeer::GetAriaRole() const {
  try {
    return DynamicAutomationProperties::GetAriaRole(Owner());
  } catch (...) {
  }

  return winrt::Microsoft::ReactNative::AriaRole::Unknown;
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
      case winrt::Microsoft::ReactNative::AccessibilityStates::Busy:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateBusyProperty());
        break;
      case winrt::Microsoft::ReactNative::AccessibilityStates::Expanded:
        value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateExpandedProperty());
        break;
    }
    return (value != xaml::DependencyProperty::UnsetValue());
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
