#include "pch.h"
#include "MainPage.h"
#if __has_include("MainPage.g.cpp")
#include "MainPage.g.cpp"
#endif

#include "App.h"

// clang-format off

using namespace winrt;
using namespace <%=XamlNamespaceCpp%>;

namespace winrt::<%=ns%>::implementation
{
    MainPage::MainPage()
    {
        InitializeComponent();
        auto app = Application::Current().as<App>();
        ReactRootView().ReactNativeHost(app->Host());
    }
}

// clang-format on
