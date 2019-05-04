#pragma once

#include "MainPage.g.h"

namespace winrt::PlaygroundCpp::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
		const winrt::hstring JSFILENAME = L"App.windows";
		const winrt::hstring JSCOMPONENTNAME = L"Playground";

        MainPage();

	private:
		void LoadReact();
    };
}

namespace winrt::PlaygroundCpp::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
