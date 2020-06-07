#pragma once

#include "App.xaml.g.h"

// clang-format off
namespace winrt::{{ namespaceCpp }}::implementation
{
    struct App : AppT<App>
    {
        App() noexcept;
        void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&);
        void OnSuspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const&);
        void OnNavigationFailed(IInspectable const&, {{ xamlNamespaceCpp }}::Navigation::NavigationFailedEventArgs const&);
      private:
        using super = AppT<App>;
    };
} // namespace winrt::{{ namespaceCpp }}::implementation

// clang-format on
