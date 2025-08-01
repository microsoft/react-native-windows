// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "AccessibilityInfoModule.h"
#ifndef USE_FABRIC
#include <UI.Xaml.Automation.Peers.h>
#include <UI.Xaml.Controls.h>
#include <XamlUtils.h>
#else
#include <Fabric/Composition/ReactNativeIsland.h>
#endif
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

void AccessibilityInfo::isReduceMotionEnabled(std::function<void(bool)> const &onSuccess) noexcept {
  m_context.UIDispatcher().Post([weakThis = weak_from_this(), onSuccess] {
    if (auto strongThis = weakThis.lock()) {
      winrt::Windows::UI::ViewManagement::UISettings uiSettings;
      auto animationsEnabled = uiSettings.AnimationsEnabled();
      onSuccess(!animationsEnabled);
    }
  });
}

void AccessibilityInfo::isInvertColorsEnabled(std::function<void(bool)> const &onSuccess) noexcept {
  // TODO: implement ##14206
  onSuccess(false);
}

void AccessibilityInfo::isHighTextContrastEnabled(std::function<void(bool)> const &onSuccess) noexcept {
  // TODO: implement #14099
  onSuccess(false);
}

void AccessibilityInfo::isTouchExplorationEnabled(std::function<void(bool)> const &onSuccess) noexcept {
  onSuccess(UiaClientsAreListening());
}

void AccessibilityInfo::setAccessibilityFocus(double /*reactTag*/) noexcept {
  // no-op - This appears to be unused in RN
}

void AccessibilityInfo::announceForAccessibility(std::wstring announcement) noexcept {
  m_context.UIDispatcher().Post([context = m_context, announcement = std::move(announcement)] {
#ifndef USE_FABRIC
    // Windows requires a specific element to announce from. Unfortunately the react-native API does not provide a tag
    // So we need to find something to raise the notification event from.
    xaml::UIElement element{nullptr};

    if (IsXamlIsland()) {
      if (auto accessibleRoot =
              winrt::Microsoft::ReactNative::XamlUIService::GetAccessibleRoot(context.Properties().Handle())) {
        element = accessibleRoot;
      } else {
        return;
      }
    } else {
      element = xaml::Controls::TextBlock();
    }

    auto peer = xaml::Automation::Peers::FrameworkElementAutomationPeer::FromElement(element);

    if (!peer) {
      return;
    }

    winrt::hstring hstr{announcement};
    peer.RaiseNotificationEvent(
        xaml::Automation::Peers::AutomationNotificationKind::Other,
        xaml::Automation::Peers::AutomationNotificationProcessing::ImportantMostRecent,
        hstr,
        hstr);
#else
    if (auto reactNativeIsland = context.Properties().Get(
            winrt::Microsoft::ReactNative::implementation::ReactNativeIsland::LastFocusedReactNativeIslandProperty())) {
      // Now you have access to the ReactNativeIsland
      if (auto uiaprovider = reactNativeIsland.GetUiaProvider()) {
        if (auto rawProvider = uiaprovider.try_as<IRawElementProviderSimple>()) {
          // Convert announcement to BSTR for UIA
          winrt::hstring hstrAnnouncement{announcement};
          auto bstrAnnouncement = SysAllocString(hstrAnnouncement.c_str());

          if (bstrAnnouncement) {
            // Raise the UIA notification event
            HRESULT hr = UiaRaiseNotificationEvent(
                rawProvider.get(),
                NotificationKind_Other,
                NotificationProcessing_ImportantMostRecent,
                bstrAnnouncement,
                bstrAnnouncement);

            // Clean up BSTRs
            SysFreeString(bstrAnnouncement);
          }
        }
      }
    }

#endif
  });
}

void AccessibilityInfo::getRecommendedTimeoutMillis(
    double mSec,
    std::function<void(double)> const &onSuccess) noexcept {
  onSuccess(mSec);
}

void AccessibilityInfo::isGrayscaleEnabled(std::function<void(bool)> const &onSuccess) noexcept {
  // TODO: implement #14207
  onSuccess(false);
}

void AccessibilityInfo::isAccessibilityServiceEnabled(std::function<void(bool)> const &onSuccess) noexcept {
  onSuccess(false);
}

} // namespace Microsoft::ReactNative
