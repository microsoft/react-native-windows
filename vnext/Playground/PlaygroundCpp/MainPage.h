// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "MainPage.g.h"

namespace winrt::PlaygroundCpp::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
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
