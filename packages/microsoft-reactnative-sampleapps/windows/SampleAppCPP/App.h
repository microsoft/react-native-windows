#pragma once
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "App.xaml.g.h"

#include "MainReactNativeHost.h"

namespace winrt::SampleApp::implementation {
struct App : AppT<App> {
  App();

  winrt::Microsoft::ReactNative::ReactNativeHost HostCore() {
    if (m_host == nullptr) {
      m_host =
          winrt::make<winrt::SampleApp::implementation::MainReactNativeHost>();
    }

    return m_host;
  }

 private:
  winrt::SampleApp::MainReactNativeHost m_host{};
};
} // namespace winrt::SampleApp::implementation
