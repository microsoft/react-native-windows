#pragma once

#include "MainPage.g.h"

// clang-format off

namespace winrt::<%=ns%>::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

    private:
      void LoadReact();
    };
}

namespace winrt::<%=ns%>::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}

// clang-format on
