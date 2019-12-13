// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ViewPanel.h>

#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.h>

#include "ViewControl.g.h"
namespace winrt::react::uwp::implementation {

//
// ViewControl is a ContentControl that ViewViewManager uses to wrap a ViewPanel
// when we want that ViewPanel to be keyboard focusable
//
struct ViewControl : ViewControlT<ViewControl> {
  using Super = ViewControlT<ViewControl>;

 public:
  // Constructors
  ViewControl();

  winrt::Windows::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer();

  winrt::react::uwp::ViewPanel GetPanel() const;
};

} // namespace winrt::react::uwp::implementation

namespace winrt::react::uwp::factory_implementation {
struct ViewControl : ViewControlT<ViewControl, implementation::ViewControl> {};
} // namespace winrt::react::uwp::factory_implementation
