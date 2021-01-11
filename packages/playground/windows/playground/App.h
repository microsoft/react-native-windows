#pragma once
#include "App.xaml.g.h"

#ifdef USE_WINUI3
namespace activation = winrt::Microsoft::UI::Xaml;
#else
namespace activation = winrt::Windows::ApplicationModel::Activation;
#endif

namespace winrt::playground::implementation {
struct App : AppT<App> {
  App();

  void OnLaunched(activation::LaunchActivatedEventArgs const &);
  void OnSuspending(IInspectable const &, Windows::ApplicationModel::SuspendingEventArgs const &);
  void OnNavigationFailed(IInspectable const &, xaml::Navigation::NavigationFailedEventArgs const &);
};
} // namespace winrt::playground::implementation
