#include "pch.h"
#include "MainPage.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

#include <winrt/Microsoft.ReactNative.h>
#include <App.h>

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::SampleAppCpp::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
        ReactRootView().ReactNativeHost(Application::Current().as<App>()->Host());
    }

}
