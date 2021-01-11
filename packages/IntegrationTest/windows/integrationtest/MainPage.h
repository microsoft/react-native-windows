#pragma once
#include "MainPage.g.h"
#include <winrt/Microsoft.ReactNative.h>

#include "TestHostHarness.h"

namespace winrt::integrationtest::implementation {
struct MainPage : MainPageT<MainPage> {
  MainPage();
};
} // namespace winrt::integrationtest::implementation

namespace winrt::integrationtest::factory_implementation {
struct MainPage : MainPageT<MainPage, implementation::MainPage> {};
} // namespace winrt::integrationtest::factory_implementation
