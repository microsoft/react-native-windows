// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include "App.xaml.g.h"
#include <winrt/Windows.ApplicationModel.Activation.h>

namespace winrt::SampleAppCpp::implementation {

struct App : AppT<App> {
  App() noexcept;
  void OnLaunched(winrt::Windows::ApplicationModel::Activation::LaunchActivatedEventArgs);

 private:
     using base = AppT;
};

} // namespace winrt::SampleAppCpp::implementation
