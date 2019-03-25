// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <Views/ShadowNodeBase.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace react {
namespace uwp {

//
// ViewControl is ViewViewManager's ContentControl but with a custom AutomationPeer
// using DynamicAutomationPeer (See below)
//
struct ViewControl : winrt::Windows::UI::Xaml::Controls::ContentControlT<ViewControl>
{
  using Super = winrt::Windows::UI::Xaml::Controls::ContentControlT<ViewControl>;
private:
  // Constructors
  ViewControl();

public:
  static winrt::com_ptr<ViewControl> Create();
  template <typename D, typename... Args> friend auto winrt::make_self(Args&&... args);

  winrt::Windows::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer();

  // Public Methods
  AccessibilityRoles AccessibilityRole() { return m_accessibilityRole; }
  void AccessibilityRole(AccessibilityRoles role) { m_accessibilityRole = role; }

  using AccessibilityInvokeEventHandler = std::function<void(void)>;
  const AccessibilityInvokeEventHandler& AccessibilityInvoke() { return m_accessibilityInvokeHandler; }
  void AccessibilityInvoke(AccessibilityInvokeEventHandler&& handler)
  {
    m_accessibilityInvokeHandler = std::move(handler);
  }

private:
  AccessibilityRoles m_accessibilityRole = AccessibilityRoles::None;
  AccessibilityInvokeEventHandler m_accessibilityInvokeHandler = nullptr;
};

}} // namespace react::uwp


#include "cppwinrt/react.uwp.DynamicAutomationPeer.g.h"
namespace winrt::react::uwp::implementation
{
//
// DynamicAutomationPeer refers to the owner ViewControl to determine what type control
// it appears to be for accessibility tools
//
struct DynamicAutomationPeer : DynamicAutomationPeerT<DynamicAutomationPeer>
{
  using Super = DynamicAutomationPeerT<DynamicAutomationPeer>;

  DynamicAutomationPeer() = delete;
  DynamicAutomationPeer(Windows::UI::Xaml::FrameworkElement const& owner);

  Windows::UI::Xaml::Automation::Peers::AutomationControlType GetAutomationControlTypeCore() const;
  Windows::Foundation::IInspectable GetPatternCore(Windows::UI::Xaml::Automation::Peers::PatternInterface const& patternInterface) const;

  void Invoke() const;
};
} // namespace winrt::react::uwp::implementation

