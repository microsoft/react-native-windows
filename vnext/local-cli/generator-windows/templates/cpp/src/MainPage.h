#pragma once
#include "MainPage.g.h"
#include <winrt/Microsoft.ReactNative.h>

// clang-format off
namespace winrt::<%=ns%>::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();
    };
}

namespace winrt::<%=ns%>::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}

// clang-format on
