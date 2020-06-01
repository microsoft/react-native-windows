// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MainPage.h"
#include "MainPage.g.cpp"
#include <CppWinRTIncludes.h>
#include <winrt/Microsoft.ReactNative.h>
#include "App.h"

using namespace winrt;
using namespace xaml;

namespace winrt::SampleAppCpp::implementation {
MainPage::MainPage() {
  InitializeComponent();
  ReactRootView().ReactNativeHost(Application::Current().as<App>()->Host());
}

} // namespace winrt::SampleAppCpp::implementation
