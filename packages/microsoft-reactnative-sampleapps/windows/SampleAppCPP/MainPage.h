// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "MainPage.g.h"

namespace winrt::SampleAppCpp::implementation {
struct MainPage : MainPageT<MainPage> {
  MainPage();
};
} // namespace winrt::SampleAppCpp::implementation

namespace winrt::SampleAppCpp::factory_implementation {
struct MainPage : MainPageT<MainPage, implementation::MainPage> {};
} // namespace winrt::SampleAppCpp::factory_implementation
