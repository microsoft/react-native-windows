// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationPeer.h"
#include "ViewControl.h"

namespace winrt {
using namespace Windows::UI::Xaml::Automation::Peers;
using namespace Windows::UI::Xaml;
} // namespace winrt

namespace winrt::react::uwp::implementation {

ViewControl::ViewControl() : Super() {
  VerticalContentAlignment(winrt::VerticalAlignment::Stretch);
  HorizontalContentAlignment(winrt::HorizontalAlignment::Stretch);
}

winrt::AutomationPeer ViewControl::OnCreateAutomationPeer() {
  return winrt::make<winrt::react::uwp::implementation::DynamicAutomationPeer>(*this);
}

winrt::react::uwp::ViewPanel ViewControl::GetPanel() const {
  auto child = Content();

  if (auto border = child.try_as<winrt::Windows::UI::Xaml::Controls::Border>()) {
    child = border.Child();
  }

  auto panel = child.try_as<winrt::react::uwp::ViewPanel>();
  assert(panel != nullptr);

  return panel;
}

} // namespace winrt::react::uwp::implementation
