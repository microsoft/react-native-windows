// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "CoreAppPage.h"
#include "ReactApplication.h"

#if __has_include("CoreAppPage.g.cpp")
#include "CoreAppPage.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::Microsoft::ReactNative::implementation {
CoreAppPage::CoreAppPage() {
  InitializeComponent();
  auto app = xaml::Application::Current().as<winrt::Microsoft::ReactNative::ReactApplication>();
  auto impl_app = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactApplication>(app);
  auto page_navigated = impl_app->PageNavigatedInternal();
  page_navigated(app, ReactRootView());
  ReactRootView().ReactNativeHost(app.Host());
}

} // namespace winrt::Microsoft::ReactNative::implementation
