// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewControl.h"
#include "DynamicAutomationPeer.h"

namespace winrt {
  using namespace Windows::UI::Xaml::Automation::Peers;
}

namespace winrt::react::uwp::implementation
{

ViewControl::ViewControl() : Super()
{
}

winrt::AutomationPeer ViewControl::OnCreateAutomationPeer()
{
  return winrt::make<winrt::react::uwp::implementation::DynamicAutomationPeer>(*this);
}

winrt::react::uwp::ViewPanel ViewControl::GetPanel() const
{
  auto child = Content();

  if (auto border = child.try_as<winrt::Windows::UI::Xaml::Controls::Border>())
  {
    child = border.Child();
  }

  auto panel = child.try_as<winrt::react::uwp::ViewPanel>();
  assert(panel != nullptr);

  return panel;
}

}
