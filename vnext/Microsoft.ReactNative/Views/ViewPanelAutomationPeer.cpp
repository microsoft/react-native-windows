// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ViewPanelAutomationPeer.h"
#include "ViewPanelAutomationPeer.g.cpp"

#include <winrt/Windows.UI.Xaml.Interop.h>

namespace winrt
{
    using namespace winrt::Windows::Foundation;
    using namespace winrt::Windows::Foundation::Collections;
    using namespace winrt::Windows::UI::Xaml;
    using namespace winrt::Windows::UI::Xaml::Interop;
    using namespace winrt::Windows::UI::Xaml::Automation::Peers;
} // namespace winrt

namespace winrt::Microsoft::ReactNative::implementation {
    ViewPanelAutomationPeer::ViewPanelAutomationPeer(winrt::FrameworkElement const& owner) : Super(owner) {}

    void ViewPanelAutomationPeer::HideChildren(bool value)
    {
        if (m_hideChildren != value) {
            m_hideChildren = value;
            InvalidatePeer();
        }
    }

    winrt::hstring ViewPanelAutomationPeer::GetClassNameCore() const
    {
        return L"ViewPanelAutomationPeer";
    }

    winrt::IVector<winrt::AutomationPeer> ViewPanelAutomationPeer::GetChildrenCore()
    {
        if (m_hideChildren) {
            return winrt::single_threaded_vector<winrt::AutomationPeer>();
        }
        else {
            return Super::GetChildrenCore();
        }
    }
} // namespace winrt::Microsoft::ReactNative::implementation
