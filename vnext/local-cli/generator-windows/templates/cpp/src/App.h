#pragma once

#include "App.xaml.g.h"

// clang-format off
namespace winrt::<%=ns%>::implementation
{
    struct App : AppT<App>
    {
        App() noexcept;
        void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&);
        void OnSuspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const&);
        void OnNavigationFailed(IInspectable const&, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const&);
    };
} // namespace winrt::<%=ns%>::implementation

// clang-format on
