// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewControl.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt {
using namespace Windows::UI::Xaml::Interop;
}

namespace react::uwp
{

  ViewControl::ViewControl()
  {
  }

  /*static*/ winrt::com_ptr<ViewControl> ViewControl::Create()
  {
    return winrt::make_self<ViewControl>();
  }

  winrt::AutomationPeer ViewControl::OnCreateAutomationPeer()
  {
    auto dyn = winrt::make<winrt::react::uwp::implementation::DynamicAutomationPeer>(*this);
    return dyn;
  }

}

namespace winrt::react::uwp::implementation
{

DynamicAutomationPeer::DynamicAutomationPeer(winrt::FrameworkElement const& owner)
  : Super(owner)
{
}

winrt::AutomationControlType DynamicAutomationPeer::GetAutomationControlTypeCore() const
{
  if (auto viewControl = GetViewControl())
  {
    switch (viewControl->AccessibilityRole())
    {
    case ::react::uwp::AccessibilityRoles::Button:
    case ::react::uwp::AccessibilityRoles::ImageButton:
      return winrt::AutomationControlType::Button;
    case ::react::uwp::AccessibilityRoles::Link:
      return winrt::AutomationControlType::Hyperlink;
    case ::react::uwp::AccessibilityRoles::Image:
      return winrt::AutomationControlType::Image;
    case ::react::uwp::AccessibilityRoles::KeyboardKey:
      return winrt::AutomationControlType::Custom;
    case ::react::uwp::AccessibilityRoles::Text:
    case ::react::uwp::AccessibilityRoles::Summary:
    case ::react::uwp::AccessibilityRoles::Header:
      return winrt::AutomationControlType::Text;
    case ::react::uwp::AccessibilityRoles::Adjustable:
      return winrt::AutomationControlType::Slider;
    case ::react::uwp::AccessibilityRoles::Search:
    case ::react::uwp::AccessibilityRoles::Unknown:
    default:
      return winrt::AutomationControlType::Group;
    }
  }

  return winrt::AutomationControlType::Group;
}

winrt::IInspectable DynamicAutomationPeer::GetPatternCore(winrt::PatternInterface const& patternInterface) const
{
  if (auto viewControl = GetViewControl())
  {
    auto accessibilityRole = viewControl->AccessibilityRole();

    if (patternInterface == winrt::PatternInterface::Invoke &&
      (accessibilityRole == ::react::uwp::AccessibilityRoles::Button || accessibilityRole == ::react::uwp::AccessibilityRoles::ImageButton))
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
  bool disabled = false;

  if (auto viewControl = GetViewControl())
  {
    disabled = viewControl->AccessibilityState(::react::uwp::AccessibilityStates::Disabled);
  }

  return !disabled && Super::IsEnabledCore();
}

// IInvokeProvider

void DynamicAutomationPeer::Invoke() const
{
  if (auto viewControl = GetViewControl())
  {
    if (auto invokeHandler = viewControl->AccessibilityInvoke())
    {
      invokeHandler();
    }
  }
}

// ISelectionProvider

winrt::com_array<winrt::IRawElementProviderSimple> DynamicAutomationPeer::GetSelection() const
{
  // Temporarily commenting this out as it keeps crashing andperhaps isn't strictly necessary
  /*if (auto viewPanel = GetViewPanel())
  {
    try
    {
      std::vector<winrt::IRawElementProviderSimple> providers;

      for (winrt::UIElement child : viewPanel->Children())
      {
        if (auto viewControl = child.try_as<::react::uwp::ViewControl>())
        {
          if (auto peer = winrt::FrameworkElementAutomationPeer::CreatePeerForElement(child))
          {
            winrt::IRawElementProviderSimple provider = ProviderFromPeer(peer);
            providers.push_back(provider);
          }
        }
      }

      if (!providers.empty())
      {
        winrt::com_array<winrt::IRawElementProviderSimple> providersArray{ providers };
        return providersArray;
      }
    }
    catch (std::exception e) {}

  }*/

  return {};
}

// ISelectionItemProvider

bool DynamicAutomationPeer::IsSelected() const
{
  auto viewControl = GetViewControl();
  return (viewControl && viewControl->AccessibilityState(::react::uwp::AccessibilityStates::Selected));
}

winrt::IRawElementProviderSimple DynamicAutomationPeer::SelectionContainer() const
{
  if (auto viewControl = GetParentViewControl())
  {
    if (auto element = viewControl.try_as<winrt::UIElement>())
    {
      if (auto peer = winrt::FrameworkElementAutomationPeer::CreatePeerForElement(element))
      {
        return ProviderFromPeer(peer);
      }
    }
  }

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

winrt::com_ptr<::react::uwp::ViewControl> DynamicAutomationPeer::GetViewControl() const
{
  try
  {
    return Owner().try_as<::react::uwp::ViewControl>();
  }
  catch (std::exception e) {}

  return nullptr;
}

winrt::com_ptr<::react::uwp::ViewPanel> DynamicAutomationPeer::GetViewPanel() const
{
  if (auto viewControl = GetViewControl())
  {
    try
    {
      auto child = viewControl->Content();

      if (auto border = child.try_as<winrt::Border>())
      {
        child = border.Child();
      }
    
      if (auto viewPanel = child.try_as<::react::uwp::ViewPanel>())
      {
        return viewPanel;
      }
    }
    catch (std::exception e) {}
  }

  return nullptr;
}

winrt::com_ptr<::react::uwp::ViewControl> DynamicAutomationPeer::GetParentViewControl() const
{
  try
  {
    auto ancestor = Owner().as<winrt::DependencyObject>();

    while (ancestor)
    {
      if (ancestor = winrt::VisualTreeHelper::GetParent(ancestor))
      {
        if (auto ancestorViewControl = ancestor.try_as<::react::uwp::ViewControl>())
        {
          return ancestorViewControl;
        }
      }
    }
  }
  catch (std::exception e) {}

  return nullptr;
}

} // namespace winrt::react::uwp::implementation
