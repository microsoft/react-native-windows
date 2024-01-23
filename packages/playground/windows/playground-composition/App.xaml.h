// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "App.xaml.g.h"

#include <winrt/Microsoft.UI.Xaml.Hosting.h>

namespace winrt::Playground::implementation {
struct App : AppT<App> {
  App() : m_windowsXamlManager(winrt::Microsoft::UI::Xaml::Hosting::WindowsXamlManager::InitializeForCurrentThread()) {}

  void OnLaunched(winrt::Microsoft::UI::Xaml::LaunchActivatedEventArgs const &);

 private:
  winrt::Microsoft::UI::Xaml::Hosting::WindowsXamlManager m_windowsXamlManager{nullptr};
};
} // namespace winrt::Playground::implementation