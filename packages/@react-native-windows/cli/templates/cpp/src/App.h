#pragma once

#include "App.xaml.g.h"

{{#useWinUI3}}
namespace activation = winrt::Microsoft::UI::Xaml;
{{/useWinUI3}}
{{^useWinUI3}}
namespace activation = winrt::Windows::ApplicationModel::Activation;
{{/useWinUI3}}

namespace winrt::{{ namespaceCpp }}::implementation
{
    struct App : AppT<App>
    {
        App() noexcept;
        void OnLaunched(activation::LaunchActivatedEventArgs const&);
        void OnSuspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const&);
        void OnNavigationFailed(IInspectable const&, {{ xamlNamespaceCpp }}::Navigation::NavigationFailedEventArgs const&);
      private:
        using super = AppT<App>;
    };
} // namespace winrt::{{ namespaceCpp }}::implementation
