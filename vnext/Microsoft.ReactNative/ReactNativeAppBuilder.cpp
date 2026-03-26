// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactNativeAppBuilder.h"
#include "ReactNativeAppBuilder.g.cpp"

#include "ReactNativeWin32App.h"

#include "winrt/Microsoft.ReactNative.h"
#include "winrt/Microsoft.UI.Composition.h"
#include "winrt/Microsoft.UI.Dispatching.h"
#include "winrt/Microsoft.UI.Interop.h"
#include "winrt/Microsoft.UI.Windowing.h"

namespace winrt::Microsoft::ReactNative::implementation {
ReactNativeAppBuilder::ReactNativeAppBuilder() {
  m_reactNativeWin32App = winrt::make<implementation::ReactNativeWin32App>();
}

ReactNativeAppBuilder::~ReactNativeAppBuilder() {}

winrt::Microsoft::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::SetAppWindow(
    winrt::Microsoft::UI::Windowing::AppWindow const &appWindow) {
  m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->AppWindow(appWindow);

  return *this;
}

winrt::Microsoft::ReactNative::ReactNativeWin32App ReactNativeAppBuilder::Build() {
  return m_reactNativeWin32App;
}

} // namespace winrt::Microsoft::ReactNative::implementation
