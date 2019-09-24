#pragma once

#include "MainPage.g.h"

namespace winrt::templatecpp::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

	private:
		void LoadReact();
    };
}

namespace winrt::templatecpp::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
