// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewControl.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt {
using namespace Windows::UI::Xaml::Interop;
}

namespace react { namespace uwp {

ViewControl::ViewControl()
{
}

/*static*/ winrt::com_ptr<ViewControl> ViewControl::Create()
{
  return winrt::make_self<ViewControl>();
}

winrt::AutomationPeer ViewControl::OnCreateAutomationPeer()
{
  auto dyn = winrt::make<DynamicAutomationPeer>(*this);
  return dyn;
}

DynamicAutomationPeer::DynamicAutomationPeer(winrt::FrameworkElement const& owner)
  : Super(owner)
{
}

winrt::AutomationControlType DynamicAutomationPeer::GetAutomationControlTypeCore() const
{
  auto viewControl = Owner().try_as<ViewControl>();

  if (nullptr != viewControl)
  {
    switch (viewControl->AccessibilityRole())
    {
    case AccessibilityRoles::Button:
    case AccessibilityRoles::ImageButton:
      return winrt::AutomationControlType::Button;
    case AccessibilityRoles::Link:
      return winrt::AutomationControlType::Hyperlink;
    case AccessibilityRoles::Image:
      return winrt::AutomationControlType::Image;
    case AccessibilityRoles::KeyboardKey:
      return winrt::AutomationControlType::Custom;
    case AccessibilityRoles::Text:
    case AccessibilityRoles::Summary:
    case AccessibilityRoles::Header:
      return winrt::AutomationControlType::Text;
    case AccessibilityRoles::Adjustable:
      return winrt::AutomationControlType::Slider;
    case AccessibilityRoles::Search:
    case AccessibilityRoles::Unknown:
    default:
      return winrt::AutomationControlType::Group;
    }
  }

  return winrt::AutomationControlType::Group;
}

winrt::IInspectable DynamicAutomationPeer::GetPatternCore(winrt::PatternInterface const& patternInterface) const
{
  auto viewControl = Owner().try_as<ViewControl>();

  if (nullptr != viewControl)
  {
    auto accessibilityRole = viewControl->AccessibilityRole();

    if (patternInterface == winrt::PatternInterface::Invoke &&
      (accessibilityRole == AccessibilityRoles::Button || accessibilityRole == AccessibilityRoles::ImageButton))
    {
      return *this;
    }
    else if (patternInterface == winrt::PatternInterface::Selection || patternInterface == winrt::PatternInterface::SelectionItem)
    {
      return *this;
    }
  }

  return nullptr;
}

bool DynamicAutomationPeer::IsEnabledCore() const
{
  auto viewControl = Owner().try_as<ViewControl>();

  if (nullptr != viewControl && viewControl->AccessibilityState(AccessibilityStates::Disabled))
  {
    return false;
  }

  return Super::IsEnabledCore();
}

void DynamicAutomationPeer::Invoke() const
{
  auto viewControl = Owner().try_as<ViewControl>();

  if (nullptr != viewControl)
  {
    auto invokeHandler = viewControl->AccessibilityInvoke();
    if (invokeHandler)
      invokeHandler();
  }
}

winrt::com_array<winrt::IRawElementProviderSimple> DynamicAutomationPeer::GetSelection() const
{
  return {};
}

bool DynamicAutomationPeer::IsSelected() const
{
  auto viewControl = Owner().try_as<ViewControl>();
  return (nullptr != viewControl && viewControl->AccessibilityState(AccessibilityStates::Selected));
}

winrt::IRawElementProviderSimple DynamicAutomationPeer::SelectionContainer() const
{
  return nullptr;
}

void DynamicAutomationPeer::AddToSelection() const
{

}

void DynamicAutomationPeer::RemoveFromSelection() const
{

}

void DynamicAutomationPeer::Select() const
{

}

} } // namespace react::uwp
