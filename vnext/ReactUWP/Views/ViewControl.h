// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ShadowNodeBase.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.Provider.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace winrt {
  using namespace Windows::Foundation;
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Xaml::Automation;
  using namespace Windows::UI::Xaml::Automation::Peers;
  using namespace Windows::UI::Xaml::Automation::Provider;
  using namespace Windows::UI::Xaml::Controls;
  using namespace Windows::UI::Xaml::Media;
}

namespace react {
namespace uwp {

//
// ViewControl is ViewViewManager's ContentControl but with a custom AutomationPeer
// using DynamicAutomationPeer (See below)
//
struct ViewControl : winrt::ContentControlT<ViewControl>
{
  using Super = winrt::ContentControlT<ViewControl>;
private:
  // Constructors
  ViewControl();

public:
  static winrt::com_ptr<ViewControl> Create();
  template <typename D, typename... Args> friend auto winrt::make_self(Args&&... args);

  winrt::AutomationPeer OnCreateAutomationPeer();

  // Public Methods
  AccessibilityRoles AccessibilityRole() { return m_accessibilityRole; }
  void AccessibilityRole(AccessibilityRoles role) { m_accessibilityRole = role; }

  bool AccessibilityState(AccessibilityStates state) { return m_accessibilityStates[state]; }
  void AccessibilityState(AccessibilityStates state, bool value)
  {
    m_accessibilityStates[state] = value;
  }

  using AccessibilityInvokeEventHandler = std::function<void(void)>;
  const AccessibilityInvokeEventHandler& AccessibilityInvoke() { return m_accessibilityInvokeHandler; }
  void AccessibilityInvoke(AccessibilityInvokeEventHandler&& handler)
  {
    m_accessibilityInvokeHandler = std::move(handler);
  }

private:
  AccessibilityRoles m_accessibilityRole = AccessibilityRoles::None;
  bool m_accessibilityStates[AccessibilityStates::CountStates] = { 0 };
  AccessibilityInvokeEventHandler m_accessibilityInvokeHandler = nullptr;
};

//
// DynamicAutomationPeer refers to the owner ViewControl to determine what type control
// it appears to be for accessibility tools
//
struct DynamicAutomationPeer : winrt::FrameworkElementAutomationPeerT<DynamicAutomationPeer, winrt::IInvokeProvider>
{
  using Super = winrt::FrameworkElementAutomationPeerT<DynamicAutomationPeer, winrt::IInvokeProvider>;

  DynamicAutomationPeer() = delete;
  DynamicAutomationPeer(winrt::FrameworkElement const& owner);

  winrt::AutomationControlType GetAutomationControlTypeCore() const;
  winrt::IInspectable GetPatternCore(winrt::PatternInterface const& patternInterface) const;

  bool IsEnabledCore() const;

  // IInvokeProvider
  void Invoke() const;
};

}} // namespace react::uwp
