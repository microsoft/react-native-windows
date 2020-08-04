#pragma once
#include "MainPage.g.h"
#include <winrt/Microsoft.ReactNative.h>

// clang-format off
namespace winrt::{{ namespaceCpp }}::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
    };
}

namespace winrt::{{ namespaceCpp }}::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}

// clang-format on
