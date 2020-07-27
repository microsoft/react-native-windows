// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Utils/AccessibilityUtils.h>

#include <UI.Xaml.Automation.Peers.h>
#include <UI.Xaml.Automation.h>
#include <Views/DynamicAutomationProperties.h>

namespace react::uwp {

REACTWINDOWS_API_(void)
AnnounceLiveRegionChangedIfNeeded(const xaml::FrameworkElement &element) {
  if (xaml::Automation::AutomationProperties::GetLiveSetting(element) !=
          xaml::Automation::Peers::AutomationLiveSetting::Off &&
      !xaml::Automation::AutomationProperties::GetName(element).empty()) {
    auto peer = xaml::Automation::Peers::FrameworkElementAutomationPeer::FromElement(element);
    if (nullptr != peer) {
      peer.RaiseAutomationEvent(xaml::Automation::Peers::AutomationEvents::LiveRegionChanged);
    }
  }
}

REACTWINDOWS_API_(bool)
HasDynamicAutomationProperties(const xaml::UIElement &element) {
  static auto unsetValue = xaml::DependencyProperty::UnsetValue();

  if (element) {
    return (unsetValue != element.ReadLocalValue(DynamicAutomationProperties::AccessibilityRoleProperty()));
  }

  return false;
}
} // namespace react::uwp
