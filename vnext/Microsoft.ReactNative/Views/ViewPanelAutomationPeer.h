// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewPanelAutomationPeer.g.h"
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.h>

namespace winrt::Microsoft::ReactNative::implementation {

struct ViewPanelAutomationPeer : ViewPanelAutomationPeerT<ViewPanelAutomationPeer> {
public:
    using Super = ViewPanelAutomationPeerT<ViewPanelAutomationPeer>;

    ViewPanelAutomationPeer(winrt::Windows::UI::Xaml::FrameworkElement const& owner);

    bool HideChildren()
    {
        return m_hideChildren;
    }
    void HideChildren(bool value);

    winrt::hstring GetClassNameCore() const;
    bool IsEnabledCore() const
    {
        return false;
    }
    winrt::Windows::Foundation::Collections::IVector<winrt::Windows::UI::Xaml::Automation::Peers::AutomationPeer> GetChildrenCore();

private:
    bool m_hideChildren;
};
} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ViewPanelAutomationPeer : ViewPanelAutomationPeerT<ViewPanelAutomationPeer, implementation::ViewPanelAutomationPeer> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation