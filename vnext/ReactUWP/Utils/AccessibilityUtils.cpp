// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/AccessibilityUtils.h>

#include <Views\DynamicAutomationProperties.h>

namespace winrt {
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Automation;
using namespace Windows::UI::Xaml::Automation::Peers;
} // namespace winrt

namespace react {
namespace uwp {

REACTWINDOWS_API_(void)
AnnounceLiveRegionChangedIfNeeded(const winrt::FrameworkElement &element) {
  if (winrt::AutomationProperties::GetLiveSetting(element) !=
          winrt::AutomationLiveSetting::Off &&
      !winrt::AutomationProperties::GetName(element).empty()) {
    auto peer = winrt::FrameworkElementAutomationPeer::FromElement(element);
    if (nullptr != peer) {
      peer.RaiseAutomationEvent(winrt::AutomationEvents::LiveRegionChanged);
    }
  }
}

REACTWINDOWS_API_(bool)
HasDynamicAutomationProperties(
    const winrt::Windows::UI::Xaml::UIElement &element) {
  static auto unsetValue = winrt::DependencyProperty::UnsetValue();

  if (element) {
    return (
        unsetValue !=
        element.ReadLocalValue(
            DynamicAutomationProperties::AccessibilityRoleProperty()));
  }

  return false;
}
}
}
