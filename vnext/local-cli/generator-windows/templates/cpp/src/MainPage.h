#pragma once
#include <winrt/Microsoft.ReactNative.h>
#include "MainPage.g.h"

// clang-format off
namespace winrt::<%=ns%>::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
    };
}

namespace winrt::testcpp::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}

// clang-format on
