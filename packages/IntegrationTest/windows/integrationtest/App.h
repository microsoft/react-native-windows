#pragma once

#include "App.xaml.g.h"
#include "TestHostHarness.h"

namespace activation = winrt::Windows::ApplicationModel::Activation;

namespace winrt::integrationtest::implementation {
struct App : AppT<App> {
  App() noexcept;
  void OnLaunched(activation::LaunchActivatedEventArgs const &);
  void OnSuspending(IInspectable const &, Windows::ApplicationModel::SuspendingEventArgs const &);
  void OnNavigationFailed(IInspectable const &, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const &);
  IntegrationTest::TestHostHarness &TestHarness();

 private:
  using super = AppT<App>;

  winrt::com_ptr<IntegrationTest::TestHostHarness> m_harness;
};
} // namespace winrt::integrationtest::implementation
