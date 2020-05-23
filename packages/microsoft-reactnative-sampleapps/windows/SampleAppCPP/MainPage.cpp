#include "pch.h"
#include "MainPage.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

#include <App.h>
#include <winrt/Microsoft.ReactNative.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::SampleAppCpp::implementation {
MainPage::MainPage() {
  InitializeComponent();
  ReactRootView().ReactNativeHost(Application::Current().as<App>()->Host());
}

} // namespace winrt::SampleAppCpp::implementation
