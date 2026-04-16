// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactNativeWin32App.h"
#include "ReactNativeWin32App.g.cpp"

#include "IReactDispatcher.h"
#include "ReactCoreInjection.h"
#include "ReactNativeHost.h"

#include "winrt/Microsoft.UI.Composition.h"
#include "winrt/Microsoft.UI.Content.h"
#include "winrt/Microsoft.UI.Dispatching.h"
#include "winrt/Microsoft.UI.Interop.h"
#include "winrt/Microsoft.UI.Windowing.h"

namespace winrt::Microsoft::ReactNative::implementation {
ReactNativeWin32App::ReactNativeWin32App() {}

ReactNativeWin32App::~ReactNativeWin32App() {
  // Destroy all Composition objects
  if (m_compositor != nullptr) {
    m_compositor.Close();
    m_compositor = nullptr;
  }
}

winrt::Microsoft::UI::Windowing::AppWindow ReactNativeWin32App::AppWindow() {
  return ReactNativeWindow().AppWindow();
}

void ReactNativeWin32App::AppWindow(winrt::Microsoft::UI::Windowing::AppWindow const &appWindow) {
  m_appWindow = appWindow;
}

winrt::Microsoft::UI::Composition::Compositor ReactNativeWin32App::Compositor() {
  if (m_compositor == nullptr) {
    // Create a DispatcherQueue for this thread. This is needed for Composition, Content, and Input APIs.
    m_dispatcherQueueController = winrt::Microsoft::UI::Dispatching::DispatcherQueueController::CreateOnCurrentThread();

    // Currently set the property to use current thread dispatcher as a default UI dispatcher.
    ReactNativeHost().InstanceSettings().Properties().Set(
        ReactDispatcherHelper::UIDispatcherProperty(), ReactDispatcherHelper::UIThreadDispatcher());

    m_compositor = winrt::Microsoft::UI::Composition::Compositor();
    winrt::Microsoft::ReactNative::Composition::CompositionUIService::SetCompositor(
        ReactNativeHost().InstanceSettings(), m_compositor);
  }
  return m_compositor;
}

winrt::Microsoft::ReactNative::ReactNativeHost ReactNativeWin32App::ReactNativeHost() {
  if (m_host == nullptr) {
    m_host = winrt::make<winrt::Microsoft::ReactNative::implementation::ReactNativeHost>();
  }
  return m_host;
}

winrt::Microsoft::ReactNative::ReactViewOptions ReactNativeWin32App::ReactViewOptions() {
  if (m_reactViewOptions == nullptr) {
    m_reactViewOptions = winrt::make<winrt::Microsoft::ReactNative::implementation::ReactViewOptions>();
  }
  return m_reactViewOptions;
}

void ReactNativeWin32App::ReactViewOptions(winrt::Microsoft::ReactNative::ReactViewOptions const &viewOptions) {
  m_reactViewOptions = viewOptions;
}

winrt::Microsoft::ReactNative::ReactNativeWindow ReactNativeWin32App::ReactNativeWindow() noexcept {
  if (!m_rnWindow) {
    if (m_appWindow) {
      m_rnWindow = winrt::Microsoft::ReactNative::ReactNativeWindow::CreateFromWindow(m_appWindow, Compositor());
    } else {
      m_rnWindow = winrt::Microsoft::ReactNative::ReactNativeWindow::CreateFromCompositor(Compositor());
      m_appWindow = m_rnWindow.AppWindow();
      m_appWindow.Title(L"ReactNativeWin32App");
      m_appWindow.Resize({1000, 1000});
    }
  }

  return m_rnWindow;
}

void ReactNativeWin32App::Start() {
  // Show the hosting AppWindow
  AppWindow().Show();

  auto hwnd{winrt::Microsoft::UI::GetWindowFromWindowId(m_appWindow.Id())};

  winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
      ReactNativeHost().InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

  // Start the react-native instance, which will create a JavaScript runtime and load the applications bundle.
  ReactNativeHost().ReloadInstance();

  m_rnWindow.ReactNativeIsland().ReactViewHost(
      winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(ReactNativeHost(), ReactViewOptions()));

  // Quit application when main window is closed
  m_appWindow.Destroying([this](
                             winrt::Microsoft::UI::Windowing::AppWindow const &window,
                             winrt::Windows::Foundation::IInspectable const & /*args*/) {
    // Before we shutdown the application - unload the ReactNativeHost to give the javascript a chance to save any
    // state
    auto async = ReactNativeHost().UnloadInstance();
    async.Completed([this](auto asyncInfo, winrt::Windows::Foundation::AsyncStatus asyncStatus) {
      assert(asyncStatus == winrt::Windows::Foundation::AsyncStatus::Completed);
      ReactNativeHost().InstanceSettings().UIDispatcher().Post([]() { PostQuitMessage(0); });
    });
  });

  // Run the main application event loop
  m_dispatcherQueueController.DispatcherQueue().RunEventLoop();

  m_appWindow.Destroy();

  // Rundown the DispatcherQueue. This drains the queue and raises events to let components
  // know the message loop has finished.
  m_dispatcherQueueController.ShutdownQueue();
}
} // namespace winrt::Microsoft::ReactNative::implementation
