﻿// WARNING: Please don't edit this file. It was generated by C++/WinRT v1.0.180821.2

#pragma once
#include "winrt/impl/Windows.UI.Xaml.1.h"
#include "winrt/impl/Windows.UI.Xaml.Automation.Peers.1.h"
#include "winrt/impl/Windows.UI.Xaml.Automation.Provider.1.h"
#include "winrt/impl/react.uwp.1.h"

WINRT_EXPORT namespace winrt::react::uwp {

}

namespace winrt::impl {

}

WINRT_EXPORT namespace winrt::react::uwp {

struct WINRT_EBO DynamicAutomationPeer :
    react::uwp::IDynamicAutomationPeer,
    impl::base<DynamicAutomationPeer, Windows::UI::Xaml::Automation::Peers::FrameworkElementAutomationPeer, Windows::UI::Xaml::Automation::Peers::AutomationPeer, Windows::UI::Xaml::DependencyObject>,
    impl::require<DynamicAutomationPeer, Windows::UI::Xaml::Automation::Peers::IAutomationPeer, Windows::UI::Xaml::Automation::Peers::IAutomationPeer2, Windows::UI::Xaml::Automation::Peers::IAutomationPeer3, Windows::UI::Xaml::Automation::Peers::IAutomationPeer4, Windows::UI::Xaml::Automation::Peers::IAutomationPeer5, Windows::UI::Xaml::Automation::Peers::IAutomationPeer6, Windows::UI::Xaml::Automation::Peers::IAutomationPeer7, Windows::UI::Xaml::Automation::Peers::IAutomationPeer8, Windows::UI::Xaml::Automation::Peers::IAutomationPeer9, Windows::UI::Xaml::Automation::Peers::IAutomationPeerOverrides, Windows::UI::Xaml::Automation::Peers::IAutomationPeerOverrides2, Windows::UI::Xaml::Automation::Peers::IAutomationPeerOverrides3, Windows::UI::Xaml::Automation::Peers::IAutomationPeerOverrides4, Windows::UI::Xaml::Automation::Peers::IAutomationPeerOverrides5, Windows::UI::Xaml::Automation::Peers::IAutomationPeerOverrides6, Windows::UI::Xaml::Automation::Peers::IAutomationPeerOverrides8, Windows::UI::Xaml::Automation::Peers::IAutomationPeerOverrides9, Windows::UI::Xaml::Automation::Peers::IAutomationPeerProtected, Windows::UI::Xaml::Automation::Peers::IFrameworkElementAutomationPeer, Windows::UI::Xaml::Automation::Provider::IInvokeProvider, Windows::UI::Xaml::IDependencyObject, Windows::UI::Xaml::IDependencyObject2>
{
    DynamicAutomationPeer(std::nullptr_t) noexcept {}
    DynamicAutomationPeer(Windows::UI::Xaml::FrameworkElement const& owner);
};

}
