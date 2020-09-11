#include "pch.h"
#include "MainPage.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

#include "App.h"

using namespace winrt;
using namespace Microsoft::ReactNative;
using namespace Windows::UI::Xaml;

namespace winrt::integrationtest::implementation {
MainPage::MainPage() {
  InitializeComponent();
  auto app = Application::Current().as<App>();

  ReactRootView().ComponentName(L"IntegrationTestsApp");

  app->TestHarness().SetRootView(ReactRootView());
  ReactRootView().ReactNativeHost(app->Host());
}
} // namespace winrt::integrationtest::implementation
