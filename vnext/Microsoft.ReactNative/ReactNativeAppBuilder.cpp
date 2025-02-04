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

winrt::Microsoft::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::SetDispatcherQueueController(
    winrt::Microsoft::UI::Dispatching::DispatcherQueueController const &dispatcherQueueController) {
  m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->DispatcherQueueController(
      dispatcherQueueController);
  return *this;
}

winrt::Microsoft::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::SetCompositor(
    winrt::Microsoft::UI::Composition::Compositor const &compositor) {
  m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->Compositor(compositor);
  return *this;
}

winrt::Microsoft::ReactNative::ReactNativeAppBuilder ReactNativeAppBuilder::SetAppWindow(
    winrt::Microsoft::UI::Windowing::AppWindow const &appWindow) {
  m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->AppWindow(appWindow);

  return *this;
}

winrt::Microsoft::ReactNative::ReactNativeWin32App ReactNativeAppBuilder::Build() {
  // Create the DispatcherQueueController if the app developer doesn't provide one
  if (m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->DispatcherQueueController() == nullptr) {
    assert(m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->Compositor() == nullptr);

    // Create a DispatcherQueue for this thread. This is needed for Composition, Content, and Input APIs.
    auto dispatcherQueueController =
        winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnCurrentThread();

    m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->DispatcherQueueController(
        dispatcherQueueController);
  }

  // Create the Compositor if the app developer doesn't provide one
  if (m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->Compositor() == nullptr) {
    // Create the compositor on behalf of the App Developer.
    auto compositor = winrt::Microsoft::UI::Composition::Compositor();
    m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->Compositor(compositor);
  }

  // Create the AppWindow if the app developer doesn't provide one
  if (m_reactNativeWin32App.AppWindow() == nullptr) {
    auto appWindow = winrt::Microsoft::UI::Windowing::AppWindow::Create();
    appWindow.Title(L"ReactNativeWin32App");
    appWindow.Resize({1000, 1000});

    m_reactNativeWin32App.as<implementation::ReactNativeWin32App>().get()->AppWindow(appWindow);
  }

  return m_reactNativeWin32App;
}

} // namespace winrt::Microsoft::ReactNative::implementation
