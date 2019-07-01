// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationPeer.h"
#include "DynamicAutomationProperties.h"

#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::UI::Xaml::Automation::Provider;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
}

namespace winrt::react::uwp::implementation {

DynamicAutomationPeer::DynamicAutomationPeer(
    winrt::FrameworkElement const &owner)
    : Super(owner) {}

winrt::hstring DynamicAutomationPeer::GetClassNameCore() const {
  return L"DynamicAutomationPeer";
}

winrt::AutomationControlType
DynamicAutomationPeer::GetAutomationControlTypeCore() const {
  auto accessibilityRole = GetAccessibilityRole();

  switch (accessibilityRole) {
    case winrt::react::uwp::AccessibilityRoles::Button:
    case winrt::react::uwp::AccessibilityRoles::ImageButton:
    case winrt::react::uwp::AccessibilityRoles::Switch: // Both ToggleSwitch and
                                                        // ToggleButton return
                                                        // "Button"
      return winrt::AutomationControlType::Button;
    case winrt::react::uwp::AccessibilityRoles::Link:
      return winrt::AutomationControlType::Hyperlink;
    case winrt::react::uwp::AccessibilityRoles::Image:
      return winrt::AutomationControlType::Image;
    case winrt::react::uwp::AccessibilityRoles::KeyboardKey:
      return winrt::AutomationControlType::Custom;
    case winrt::react::uwp::AccessibilityRoles::Text:
    case winrt::react::uwp::AccessibilityRoles::Header:
    case winrt::react::uwp::AccessibilityRoles::Summary:
    case winrt::react::uwp::AccessibilityRoles::Alert:
      return winrt::AutomationControlType::Text;
    case winrt::react::uwp::AccessibilityRoles::Adjustable:
      return winrt::AutomationControlType::Slider;
    case winrt::react::uwp::AccessibilityRoles::CheckBox:
      return winrt::AutomationControlType::CheckBox;
    case winrt::react::uwp::AccessibilityRoles::ComboBox:
      return winrt::AutomationControlType::ComboBox;
    case winrt::react::uwp::AccessibilityRoles::Menu:
      return winrt::AutomationControlType::Menu;
    case winrt::react::uwp::AccessibilityRoles::MenuBar:
      return winrt::AutomationControlType::MenuBar;
    case winrt::react::uwp::AccessibilityRoles::MenuItem:
      return winrt::AutomationControlType::MenuItem;
    case winrt::react::uwp::AccessibilityRoles::ProgressBar:
      return winrt::AutomationControlType::ProgressBar;
    case winrt::react::uwp::AccessibilityRoles::Radio:
      return winrt::AutomationControlType::RadioButton;
    case winrt::react::uwp::AccessibilityRoles::ScrollBar:
      return winrt::AutomationControlType::ScrollBar;
    case winrt::react::uwp::AccessibilityRoles::SpinButton:
      return winrt::AutomationControlType::Spinner;
    case winrt::react::uwp::AccessibilityRoles::Tab:
      return winrt::AutomationControlType::TabItem;
    case winrt::react::uwp::AccessibilityRoles::TabList:
      return winrt::AutomationControlType::Tab;
    case winrt::react::uwp::AccessibilityRoles::ToolBar:
      return winrt::AutomationControlType::ToolBar;
    case winrt::react::uwp::AccessibilityRoles::None:
    case winrt::react::uwp::AccessibilityRoles::Search:
    case winrt::react::uwp::AccessibilityRoles::RadioGroup:
    case winrt::react::uwp::AccessibilityRoles::Timer:
      return winrt::AutomationControlType::Group;
  }

  return Super::GetAutomationControlTypeCore();
}

winrt::IInspectable DynamicAutomationPeer::GetPatternCore(
    winrt::PatternInterface const &patternInterface) const {
  auto accessibilityRole = GetAccessibilityRole();

  if (patternInterface == winrt::PatternInterface::Invoke &&
      (accessibilityRole == winrt::react::uwp::AccessibilityRoles::Button ||
       accessibilityRole ==
           winrt::react::uwp::AccessibilityRoles::ImageButton ||
       accessibilityRole == winrt::react::uwp::AccessibilityRoles::Radio)) {
    return *this;
  } else if (
      patternInterface == winrt::PatternInterface::Selection ||
      patternInterface == winrt::PatternInterface::SelectionItem) {
    return *this;
  } else if (
      patternInterface == winrt::PatternInterface::Toggle &&
      (accessibilityRole == winrt::react::uwp::AccessibilityRoles::CheckBox ||
       accessibilityRole == winrt::react::uwp::AccessibilityRoles::Switch ||
       accessibilityRole == winrt::react::uwp::AccessibilityRoles::Radio)) {
    return *this;
  }

  return Super::GetPatternCore(patternInterface);
}

bool DynamicAutomationPeer::IsEnabledCore() const {
  bool disabled =
      GetAccessibilityState(winrt::react::uwp::AccessibilityStates::Disabled);
  return !disabled && Super::IsEnabledCore();
}

// IInvokeProvider

void DynamicAutomationPeer::Invoke() const {
  if (auto invokeHandler = GetAccessibilityInvokeEventHandler()) {
    invokeHandler();
  }
}

// ISelectionProvider

winrt::com_array<winrt::IRawElementProviderSimple>
DynamicAutomationPeer::GetSelection() const {
  // We don't differentiate between Views that are containers and Views that are
  // items, and we don't have a way to specify the linkage between the two

  // Returning nothing until that linkage exists
  return {};
}

// ISelectionItemProvider

bool DynamicAutomationPeer::IsSelected() const {
  return GetAccessibilityState(
      winrt::react::uwp::AccessibilityStates::Selected);
}

winrt::IRawElementProviderSimple DynamicAutomationPeer::SelectionContainer()
    const {
  // We don't differentiate between Views that are containers and Views that are
  // items, and we don't have a way to specify the linkage between the two

  // Returning nothing until that linkage exists

  return nullptr;
}

void DynamicAutomationPeer::AddToSelection() const {
  // Right now RN does not have "selection" events, so this is a no-op
}

void DynamicAutomationPeer::RemoveFromSelection() const {
  // Right now RN does not have "selection" events, so this is a no-op
}

void DynamicAutomationPeer::Select() const {
  // Right now RN does not have "selection" events, so this is a no-op
}

// IToggleProvider

winrt::ToggleState DynamicAutomationPeer::ToggleState() const {
  bool checkedState =
      GetAccessibilityState(winrt::react::uwp::AccessibilityStates::Checked);
  bool uncheckedState =
      GetAccessibilityState(winrt::react::uwp::AccessibilityStates::Unchecked);

  if (!checkedState && uncheckedState) {
    return winrt::ToggleState::Off;
  } else if (checkedState && !uncheckedState) {
    return winrt::ToggleState::On;
  }

  return winrt::ToggleState::Indeterminate;
}

void DynamicAutomationPeer::Toggle() const {
  if (auto invokeHandler = GetAccessibilityInvokeEventHandler()) {
    invokeHandler();
  }
}

winrt::react::uwp::AccessibilityRoles
DynamicAutomationPeer::GetAccessibilityRole() const {
  try {
    if (auto owner = Owner()) {
      return DynamicAutomationProperties::GetAccessibilityRole(owner);
    }
  } catch (...) {
  }

  return winrt::react::uwp::AccessibilityRoles::None;
}

bool DynamicAutomationPeer::GetAccessibilityState(
    winrt::react::uwp::AccessibilityStates state) const {
  try {
    if (auto owner = Owner()) {
      switch (state) {
        case winrt::react::uwp::AccessibilityStates::Selected:
          return DynamicAutomationProperties::GetAccessibilityStateSelected(
              owner);
        case winrt::react::uwp::AccessibilityStates::Disabled:
          return DynamicAutomationProperties::GetAccessibilityStateDisabled(
              owner);
        case winrt::react::uwp::AccessibilityStates::Checked:
          return DynamicAutomationProperties::GetAccessibilityStateChecked(
              owner);
        case winrt::react::uwp::AccessibilityStates::Unchecked:
          return DynamicAutomationProperties::GetAccessibilityStateUnchecked(
              owner);
        case winrt::react::uwp::AccessibilityStates::Busy:
          return DynamicAutomationProperties::GetAccessibilityStateBusy(owner);
        case winrt::react::uwp::AccessibilityStates::Expanded:
          return DynamicAutomationProperties::GetAccessibilityStateExpanded(
              owner);
        case winrt::react::uwp::AccessibilityStates::Collapsed:
          return DynamicAutomationProperties::GetAccessibilityStateCollapsed(
              owner);
      }
    }
  } catch (...) {
  }

  return false;
}

winrt::react::uwp::AccessibilityInvokeEventHandler
DynamicAutomationPeer::GetAccessibilityInvokeEventHandler() const {
  try {
    if (auto owner = Owner()) {
      return DynamicAutomationProperties::GetAccessibilityInvokeEventHandler(
          owner);
    }
  } catch (...) {
  }

  return nullptr;
}

} // namespace winrt::react::uwp::implementation
