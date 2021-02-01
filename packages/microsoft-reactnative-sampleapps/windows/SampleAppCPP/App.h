// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "App.xaml.g.h"

namespace winrt::SampleAppCpp::implementation {

struct App : AppT<App> {
  App() noexcept;
  void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const &e);
  void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const &e);

 private:
  using super = AppT<App>;
};

} // namespace winrt::SampleAppCpp::implementation
