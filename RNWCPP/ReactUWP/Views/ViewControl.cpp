// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewControl.h"

#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.Foundation.h>

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
  auto dyn = winrt::make<winrt::react::uwp::implementation::DynamicAutomationPeer>(*this);
  return dyn;
}

} } // namespace react::uwp

namespace winrt::react::uwp::implementation
{
    DynamicAutomationPeer::DynamicAutomationPeer(Windows::UI::Xaml::FrameworkElement const& owner)
      : Super(owner)
    {
    }

    Windows::UI::Xaml::Automation::Peers::AutomationControlType DynamicAutomationPeer::GetAutomationControlTypeCore() const
    {
      auto viewControl = Owner().as<::react::uwp::ViewControl>();

      if (viewControl->AccessibilityRole() == ::react::uwp::AccessibilityRoles::Button)
        return winrt::Windows::UI::Xaml::Automation::Peers::AutomationControlType::Button;

      return winrt::Windows::UI::Xaml::Automation::Peers::AutomationControlType::Group;
    }

    Windows::Foundation::IInspectable DynamicAutomationPeer::GetPatternCore(Windows::UI::Xaml::Automation::Peers::PatternInterface const& patternInterface) const
    {
      auto viewControl = Owner().as<::react::uwp::ViewControl>();

      if (patternInterface == Windows::UI::Xaml::Automation::Peers::PatternInterface::Invoke &&
          viewControl->AccessibilityRole() == ::react::uwp::AccessibilityRoles::Button)
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

}
