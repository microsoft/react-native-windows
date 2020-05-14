// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationPeer.h"
#include "DynamicAutomationProperties.h"

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Media.h>

// Needed for latest versions of C++/WinRT
#if __has_include("DynamicAutomationPeer.g.cpp")
#include "DynamicAutomationPeer.g.cpp"
#endif

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::UI::Xaml::Automation::Provider;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

DynamicAutomationPeer::DynamicAutomationPeer(winrt::FrameworkElement const &owner) : Super(owner) {}

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
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Button:
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::ImageButton:
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Switch: // Both ToggleSwitch and
                                                                    // ToggleButton return
                                                                    // "Button"
      return winrt::AutomationControlType::Button;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Link:
      return winrt::AutomationControlType::Hyperlink;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Image:
      return winrt::AutomationControlType::Image;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::KeyboardKey:
      return winrt::AutomationControlType::Custom;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Text:
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Header:
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Summary:
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Alert:
      return winrt::AutomationControlType::Text;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Adjustable:
      return winrt::AutomationControlType::Slider;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::CheckBox:
      return winrt::AutomationControlType::CheckBox;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::ComboBox:
      return winrt::AutomationControlType::ComboBox;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Menu:
      return winrt::AutomationControlType::Menu;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::MenuBar:
      return winrt::AutomationControlType::MenuBar;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::MenuItem:
      return winrt::AutomationControlType::MenuItem;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::ProgressBar:
      return winrt::AutomationControlType::ProgressBar;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Radio:
      return winrt::AutomationControlType::RadioButton;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::ScrollBar:
      return winrt::AutomationControlType::ScrollBar;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::SpinButton:
      return winrt::AutomationControlType::Spinner;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Tab:
      return winrt::AutomationControlType::TabItem;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::TabList:
      return winrt::AutomationControlType::Tab;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::ToolBar:
      return winrt::AutomationControlType::ToolBar;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::List:
      return winrt::AutomationControlType::List;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::ListItem:
      return winrt::AutomationControlType::ListItem;
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::None:
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Search:
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::RadioGroup:
    case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Timer:
      return winrt::AutomationControlType::Group;
  }

  return Super::GetAutomationControlTypeCore();
}

winrt::IInspectable DynamicAutomationPeer::GetPatternCore(winrt::PatternInterface const &patternInterface) const {
  auto accessibilityRole = GetAccessibilityRole();

  if (patternInterface == winrt::PatternInterface::Invoke &&
      (accessibilityRole == winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Button ||
       accessibilityRole == winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::ImageButton ||
       accessibilityRole == winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Radio)) {
    return *this;
  } else if (
      (patternInterface == winrt::PatternInterface::Selection ||
       patternInterface == winrt::PatternInterface::SelectionItem) &&
      HasAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Selected)) {
    return *this;
  } else if (
      patternInterface == winrt::PatternInterface::Toggle &&
      (accessibilityRole == winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::CheckBox ||
       accessibilityRole == winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Switch ||
       accessibilityRole == winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::Radio) &&
      (HasAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Checked) ||
       HasAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Unchecked))) {
    return *this;
  } else if (
      patternInterface == winrt::PatternInterface::ExpandCollapse &&
      (HasAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Expanded) ||
       HasAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Collapsed))) {
    return *this;
  }

  return Super::GetPatternCore(patternInterface);
}

bool DynamicAutomationPeer::IsEnabledCore() const {
  bool disabled = HasAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Disabled) &&
      GetAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Disabled);
  return !disabled && Super::IsEnabledCore();
}

winrt::hstring DynamicAutomationPeer::GetItemStatusCore() const {
  winrt::hstring itemStatus = Super::GetItemStatusCore();

  if (itemStatus.empty()) {
    if (HasAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Busy) &&
        GetAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Busy)) {
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
  return GetAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Selected);
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
  bool checkedState = GetAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Checked);
  bool uncheckedState = GetAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Unchecked);

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
  bool expandedState = GetAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Expanded);
  bool collapsedState = GetAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Collapsed);

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

// Private Methods

winrt::hstring DynamicAutomationPeer::GetContentName() const {
  winrt::hstring name = L"";

  try {
    if (auto const &owner = Owner()) {
      if (auto const &viewControl = owner.try_as<winrt::PROJECT_ROOT_NAMESPACE::ViewControl>()) {
        auto viewPanel = viewControl.GetPanel();

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

winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles DynamicAutomationPeer::GetAccessibilityRole() const {
  try {
    if (auto const &owner = Owner()) {
      return DynamicAutomationProperties::GetAccessibilityRole(owner);
    }
  } catch (...) {
  }

  return winrt::PROJECT_ROOT_NAMESPACE::AccessibilityRoles::None;
}

bool DynamicAutomationPeer::HasAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates state) const {
  try {
    if (auto const &owner = Owner()) {
      winrt::IInspectable value = nullptr;
      switch (state) {
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Selected:
          value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateSelectedProperty());
          break;
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Disabled:
          value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateDisabledProperty());
          break;
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Checked:
          value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateCheckedProperty());
          break;
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Unchecked:
          value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateUncheckedProperty());
          break;
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Busy:
          value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateBusyProperty());
          break;
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Expanded:
          value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateExpandedProperty());
          break;
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Collapsed:
          value = owner.ReadLocalValue(DynamicAutomationProperties::AccessibilityStateCollapsedProperty());
          break;
      }
      return (value != winrt::DependencyProperty::UnsetValue());
    }
  } catch (...) {
  }

  return false;
}

bool DynamicAutomationPeer::GetAccessibilityState(winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates state) const {
  try {
    if (auto const &owner = Owner()) {
      switch (state) {
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Selected:
          return DynamicAutomationProperties::GetAccessibilityStateSelected(owner);
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Disabled:
          return DynamicAutomationProperties::GetAccessibilityStateDisabled(owner);
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Checked:
          return DynamicAutomationProperties::GetAccessibilityStateChecked(owner);
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Unchecked:
          return DynamicAutomationProperties::GetAccessibilityStateUnchecked(owner);
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Busy:
          return DynamicAutomationProperties::GetAccessibilityStateBusy(owner);
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Expanded:
          return DynamicAutomationProperties::GetAccessibilityStateExpanded(owner);
        case winrt::PROJECT_ROOT_NAMESPACE::AccessibilityStates::Collapsed:
          return DynamicAutomationProperties::GetAccessibilityStateCollapsed(owner);
      }
    }
  } catch (...) {
  }

  return false;
}

winrt::PROJECT_ROOT_NAMESPACE::AccessibilityInvokeEventHandler
DynamicAutomationPeer::GetAccessibilityInvokeEventHandler() const {
  try {
    if (auto const &owner = Owner()) {
      return DynamicAutomationProperties::GetAccessibilityInvokeEventHandler(owner);
    }
  } catch (...) {
  }

  return nullptr;
}

} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation
