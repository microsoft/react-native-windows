#pragma once
#include "App.xaml.g.h"

namespace winrt::playground::implementation {
struct App : AppT<App> {
  App();

  void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const &);
  void OnSuspending(IInspectable const &, Windows::ApplicationModel::SuspendingEventArgs const &);
  void OnNavigationFailed(IInspectable const &, xaml::Navigation::NavigationFailedEventArgs const &);
};
} // namespace winrt::playground::implementation
