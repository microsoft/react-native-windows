// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#include "pch.h"
#include "CoreAppPage.h"
#include "ReactApplication.h"

#if __has_include("CoreAppPage.g.cpp")
#include "CoreAppPage.g.cpp"
#endif

namespace winrt::Microsoft::ReactNative::implementation {
CoreAppPage::CoreAppPage() {
  InitializeComponent();
  auto app = xaml::Application::Current().as<winrt::Microsoft::ReactNative::ReactApplication>();
  auto implApp = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactApplication>(app);
  auto pageNavigated = implApp->PageNavigatedInternal();
  pageNavigated(app, ReactRootView());
  ReactRootView().ReactNativeHost(app.Host());
}

} // namespace winrt::Microsoft::ReactNative::implementation
