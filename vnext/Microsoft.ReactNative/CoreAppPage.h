#pragma once
#include <winrt/Windows.UI.Xaml.Markup.h>
#include "CoreAppPage.g.h"

namespace winrt::Microsoft::ReactNative::implementation
{
    struct CoreAppPage : CoreAppPageT<CoreAppPage>
    {
        CoreAppPage();
    };
}

namespace winrt::Microsoft::ReactNative::factory_implementation
{
    struct CoreAppPage : CoreAppPageT<CoreAppPage, implementation::CoreAppPage>
    {
    };
}
