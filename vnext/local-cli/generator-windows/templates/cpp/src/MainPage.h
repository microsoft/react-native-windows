#pragma once

#include "MainPage.g.h"
#include <winrt/Microsoft.ReactNative.h>

namespace winrt::<%=name%>::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        winrt::Microsoft::ReactNative::ReactRootView MainPage::ReactRootView();
        void MainPage::ReactRootView(const winrt::Microsoft::ReactNative::ReactRootView &);

        private:
            winrt::Microsoft::ReactNative::ReactRootView m_reactRootView;
    };
}

namespace winrt::<%=name%>::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}
