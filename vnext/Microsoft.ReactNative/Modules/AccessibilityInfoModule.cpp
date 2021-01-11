// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "AccessibilityInfoModule.h"
#include <UI.Xaml.Automation.Peers.h>
#include <UI.Xaml.Controls.h>
#include <XamlUtils.h>
#include <uiautomationcore.h>
#include <uiautomationcoreapi.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include "Unicode.h"
#include "Utils/Helpers.h"

namespace Microsoft::ReactNative {

void AccessibilityInfo::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

void AccessibilityInfo::isReduceMotionEnabled(std::function<void(React::JSValue const &)> const &onSuccess) noexcept {
  auto jsDispatcher = m_context.JSDispatcher();
  m_context.UIDispatcher().Post([weakThis = weak_from_this(), jsDispatcher, onSuccess] {
    if (auto strongThis = weakThis.lock()) {
      winrt::Windows::UI::ViewManagement::UISettings uiSettings;
      auto animationsEnabled = uiSettings.AnimationsEnabled();
      jsDispatcher.Post([animationsEnabled, onSuccess] { onSuccess(!animationsEnabled); });
    }
  });
}

void AccessibilityInfo::isTouchExplorationEnabled(
    std::function<void(React::JSValue const &)> const &onSuccess) noexcept {
  onSuccess(UiaClientsAreListening());
}

void AccessibilityInfo::setAccessibilityFocus(double /*reactTag*/) noexcept {
  // no-op - This appears to be unused in RN
}

void AccessibilityInfo::announceForAccessibility(std::string announcement) noexcept {
  m_context.UIDispatcher().Post([context = m_context, announcement = std::move(announcement)] {
    xaml::UIElement element{nullptr};

    // Windows requires a specific element to announce from.  Unfortunately the react-native API does not provide a tag
    // So we need to find something to try to raise the notification event from

    if (auto window = xaml::Window::Current()) {
      element = window.Content();
    }

    if (!element && react::uwp::Is19H1OrHigher()) {
      // XamlRoot added in 19H1
      if (auto xamlRoot = React::XamlUIService::GetXamlRoot(context.Properties().Handle())) {
        element = xamlRoot.Content();
      }
    }

    if (!element) {
      return;
    }

    auto peer = xaml::Automation::Peers::FrameworkElementAutomationPeer::FromElement(element);

    if (!peer) {
      return;
    }

    winrt::hstring hstr{Microsoft::Common::Unicode::Utf8ToUtf16(announcement)};
    peer.RaiseNotificationEvent(
        xaml::Automation::Peers::AutomationNotificationKind::Other,
        xaml::Automation::Peers::AutomationNotificationProcessing::ImportantMostRecent,
        hstr,
        hstr);
  });
}

} // namespace Microsoft::ReactNative
