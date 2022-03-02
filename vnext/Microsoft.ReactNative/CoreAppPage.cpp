#include "pch.h"
#include "CoreAppPage.h"


#if __has_include("CoreAppPage.g.cpp")
#include "CoreAppPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Microsoft::ReactNative::implementation
{
    CoreAppPage::CoreAppPage()
    {
        InitializeComponent();
        auto app = Application::Current().as<CoreApp>();
        ReactRootView().ReactNativeHost(app.Host());
    }

    
}
