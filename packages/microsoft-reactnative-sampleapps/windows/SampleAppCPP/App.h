// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include "App.xaml.g.h"

namespace winrt::SampleAppCpp::implementation {

struct App : AppT<App> {
  App() noexcept;
  void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const &);

 private:
  using super = AppT<App>;
};

} // namespace winrt::SampleAppCpp::implementation
