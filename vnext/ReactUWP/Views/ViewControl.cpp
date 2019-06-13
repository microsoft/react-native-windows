// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewControl.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Foundation;
}

namespace react { namespace uwp {

ViewControl::ViewControl()
{
}

/*static*/ winrt::com_ptr<ViewControl> ViewControl::Create()
{
  return winrt::make_self<ViewControl>();
}

winrt::Windows::UI::Xaml::Automation::Peers::AutomationPeer ViewControl::OnCreateAutomationPeer()
{
  auto dyn = winrt::make<DynamicAutomationPeer>(*this);
  return dyn;
}

DynamicAutomationPeer::DynamicAutomationPeer(winrt::Windows::UI::Xaml::FrameworkElement const& owner)
  : Super(owner)
{
}

winrt::Windows::UI::Xaml::Automation::Peers::AutomationControlType DynamicAutomationPeer::GetAutomationControlTypeCore() const
{
  auto viewControl = Owner().as<::react::uwp::ViewControl>();

  switch(viewControl->AccessibilityRole())
  {
    case ::react::uwp::AccessibilityRoles::Button:
    case ::react::uwp::AccessibilityRoles::ImageButton:
      return winrt::Windows::UI::Xaml::Automation::Peers::AutomationControlType::Button;
    case ::react::uwp::AccessibilityRoles::Link:
      return winrt::Windows::UI::Xaml::Automation::Peers::AutomationControlType::Hyperlink;
    case ::react::uwp::AccessibilityRoles::Image:
      return winrt::Windows::UI::Xaml::Automation::Peers::AutomationControlType::Image;
    case ::react::uwp::AccessibilityRoles::KeyboardKey:
      return winrt::Windows::UI::Xaml::Automation::Peers::AutomationControlType::Custom;
    case ::react::uwp::AccessibilityRoles::Text:
    case ::react::uwp::AccessibilityRoles::Summary:
    case ::react::uwp::AccessibilityRoles::Header:
      return winrt::Windows::UI::Xaml::Automation::Peers::AutomationControlType::Text;
    case ::react::uwp::AccessibilityRoles::Adjustable:
      return winrt::Windows::UI::Xaml::Automation::Peers::AutomationControlType::Slider;
    case ::react::uwp::AccessibilityRoles::Search:
    case ::react::uwp::AccessibilityRoles::Unknown:
    default:
      return winrt::Windows::UI::Xaml::Automation::Peers::AutomationControlType::Group;
  }
}

winrt::Windows::Foundation::IInspectable DynamicAutomationPeer::GetPatternCore(winrt::Windows::UI::Xaml::Automation::Peers::PatternInterface const& patternInterface) const
{
  auto viewControl = Owner().as<::react::uwp::ViewControl>();

  if (patternInterface == winrt::Windows::UI::Xaml::Automation::Peers::PatternInterface::Invoke &&
      (viewControl->AccessibilityRole() == ::react::uwp::AccessibilityRoles::Button || 
        viewControl->AccessibilityRole() == ::react::uwp::AccessibilityRoles::ImageButton))
  {
    return *this;
  }

  return nullptr;
}


void DynamicAutomationPeer::Invoke() const
{
  auto viewControl = Owner().as<::react::uwp::ViewControl>();
  auto invokeHandler = viewControl->AccessibilityInvoke();
  if (invokeHandler)
    invokeHandler();
}

} } // namespace react::uwp
