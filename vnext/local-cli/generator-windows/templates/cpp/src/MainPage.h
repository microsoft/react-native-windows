#pragma once
#include "MainPage.g.h"
#include <winrt/Microsoft.ReactNative.h>

// clang-format off
namespace winrt::{{ namespace }}::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
    };
}

namespace winrt::{{ namespace }}::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}

// clang-format on
