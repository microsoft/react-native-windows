// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ViewPanel.h>

#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>

#include "cppwinrt/ViewControl.g.h"
namespace winrt::react::uwp::implementation
{

//
// ViewControl is ViewViewManager's ContentControl but with a custom AutomationPeer
// using DynamicAutomationPeer (See below)
//
struct ViewControl : ViewControlT<ViewControl>
{
  using Super = ViewControlT<ViewControl>;
public:
  // Constructors
  ViewControl();

  winrt::Windows::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer();

  winrt::react::uwp::ViewPanel GetPanel() const;
};

} // winrt::react::uwp::implementation

namespace winrt::react::uwp::factory_implementation
{
  struct ViewControl : ViewControlT<ViewControl, implementation::ViewControl>
  {
  };
}
