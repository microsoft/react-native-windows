// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationPeer.h"
#include "ViewControl.h"

// Needed for latest versions of C++/WinRT
#if __has_include("ViewControl.g.cpp")
#include "ViewControl.g.cpp"
#endif

namespace winrt {
using namespace xaml::Automation::Peers;
using namespace xaml;
} // namespace winrt

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

ViewControl::ViewControl() : Super() {
  VerticalContentAlignment(xaml::VerticalAlignment::Stretch);
  HorizontalContentAlignment(xaml::HorizontalAlignment::Stretch);
}

winrt::AutomationPeer ViewControl::OnCreateAutomationPeer() {
  return winrt::make<winrt::PROJECT_ROOT_NAMESPACE::implementation::DynamicAutomationPeer>(*this);
}

winrt::PROJECT_ROOT_NAMESPACE::ViewPanel ViewControl::GetPanel() const {
  auto child = Content();

  if (auto border = child.try_as<xaml::Controls::Border>()) {
    child = border.Child();
  }

  auto panel = child.try_as<winrt::PROJECT_ROOT_NAMESPACE::ViewPanel>();
  assert(panel != nullptr);

  return panel;
}

} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation
