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

namespace winrt::react::uwp::implementation
{

DynamicAutomationPeer::DynamicAutomationPeer(winrt::FrameworkElement const& owner)
  : Super(owner)
{
}

winrt::hstring DynamicAutomationPeer::GetClassNameCore() const
{
  return L"DynamicAutomationPeer";
}

winrt::AutomationControlType DynamicAutomationPeer::GetAutomationControlTypeCore() const
{
  auto accessibilityRole = GetAccessibilityRole();

  switch (accessibilityRole)
  {
  case winrt::react::uwp::AccessibilityRoles::Button:
  case winrt::react::uwp::AccessibilityRoles::ImageButton:
    return winrt::AutomationControlType::Button;
  case winrt::react::uwp::AccessibilityRoles::Link:
    return winrt::AutomationControlType::Hyperlink;
  case winrt::react::uwp::AccessibilityRoles::Image:
    return winrt::AutomationControlType::Image;
  case winrt::react::uwp::AccessibilityRoles::KeyboardKey:
    return winrt::AutomationControlType::Custom;
  case winrt::react::uwp::AccessibilityRoles::Text:
  case winrt::react::uwp::AccessibilityRoles::Summary:
  case winrt::react::uwp::AccessibilityRoles::Header:
    return winrt::AutomationControlType::Text;
  case winrt::react::uwp::AccessibilityRoles::Adjustable:
    return winrt::AutomationControlType::Slider;
  case winrt::react::uwp::AccessibilityRoles::Search:
  case winrt::react::uwp::AccessibilityRoles::Unknown:
  default:
    return winrt::AutomationControlType::Group;
  }
}

winrt::IInspectable DynamicAutomationPeer::GetPatternCore(winrt::PatternInterface const& patternInterface) const
{
  auto accessibilityRole = GetAccessibilityRole();

  if (patternInterface == winrt::PatternInterface::Invoke &&
    (accessibilityRole == winrt::react::uwp::AccessibilityRoles::Button || accessibilityRole == winrt::react::uwp::AccessibilityRoles::ImageButton))
  {
    return *this;
  }
  else if (patternInterface == winrt::PatternInterface::Selection || patternInterface == winrt::PatternInterface::SelectionItem)
  {
    return *this;
  }

  return Super::GetPatternCore(patternInterface);
}

bool DynamicAutomationPeer::IsEnabledCore() const
{
  bool disabled = GetAccessibilityState(winrt::react::uwp::AccessibilityStates::Disabled);
  return !disabled && Super::IsEnabledCore();
}

// IInvokeProvider

void DynamicAutomationPeer::Invoke() const
{
  if (auto invokeHandler = GetAccessibilityInvokeEventHandler())
  {
    invokeHandler();
  }
}

// ISelectionProvider

winrt::com_array<winrt::IRawElementProviderSimple> DynamicAutomationPeer::GetSelection() const
{
  // We don't differentiate between Views that are containers and Views that are items, and we don't
  // have a way to specify the linkage between the two

  // Returning nothing until that linkage exists
  return {};
}

// ISelectionItemProvider

bool DynamicAutomationPeer::IsSelected() const
{
  return GetAccessibilityState(winrt::react::uwp::AccessibilityStates::Selected);
}

winrt::IRawElementProviderSimple DynamicAutomationPeer::SelectionContainer() const
{
  // We don't differentiate between Views that are containers and Views that are items, and we don't
  // have a way to specify the linkage between the two

  // Returning nothing until that linkage exists

  return nullptr;
}

void DynamicAutomationPeer::AddToSelection() const
{
  // Right now RN does not have "selection" events, so this is a no-op
}

void DynamicAutomationPeer::RemoveFromSelection() const
{
  // Right now RN does not have "selection" events, so this is a no-op
}

void DynamicAutomationPeer::Select() const
{
  // Right now RN does not have "selection" events, so this is a no-op
}

winrt::react::uwp::AccessibilityRoles DynamicAutomationPeer::GetAccessibilityRole() const
{
  try
  {
    if (auto owner = Owner())
    {
      return DynamicAutomationProperties::GetAccessibilityRole(owner);
    }
  }
  catch (...) {}

  return winrt::react::uwp::AccessibilityRoles::None;
}

bool DynamicAutomationPeer::GetAccessibilityState(winrt::react::uwp::AccessibilityStates state) const
{
  try
  {
    if (auto owner = Owner())
    {
      switch (state)
      {
      case winrt::react::uwp::AccessibilityStates::Disabled:
        return DynamicAutomationProperties::GetAccessibilityStateDisabled(owner);
      case winrt::react::uwp::AccessibilityStates::Selected:
        return DynamicAutomationProperties::GetAccessibilityStateSelected(owner);
      }
    }
  }
  catch (...) {}

  return false;
}

winrt::react::uwp::AccessibilityInvokeEventHandler DynamicAutomationPeer::GetAccessibilityInvokeEventHandler() const
{
  try
  {
    if (auto owner = Owner())
    {
      return DynamicAutomationProperties::GetAccessibilityInvokeEventHandler(owner);
    }
  }
  catch (...) {}

  return nullptr;
}

} // namespace winrt::react::uwp::implementation
