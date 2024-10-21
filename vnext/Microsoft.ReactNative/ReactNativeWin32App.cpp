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

// Scaling factor for the window's content based on the DPI of the display where the window is located.
float ScaleFactor(HWND hwnd) noexcept {
  return GetDpiForWindow(hwnd) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
}

void UpdateRootViewSizeToAppWindow(
    winrt::Microsoft::ReactNative::ReactNativeIsland const &rootView,
    winrt::Microsoft::UI::Windowing::AppWindow const &window) {
  auto hwnd = winrt::Microsoft::UI::GetWindowFromWindowId(window.Id());
  auto scaleFactor = ScaleFactor(hwnd);
  winrt::Windows::Foundation::Size size{
      window.ClientSize().Width / scaleFactor, window.ClientSize().Height / scaleFactor};
  // Do not relayout when minimized
  if (window.Presenter().as<winrt::Microsoft::UI::Windowing::OverlappedPresenter>().State() !=
      winrt::Microsoft::UI::Windowing::OverlappedPresenterState::Minimized) {
    winrt::Microsoft::ReactNative::LayoutConstraints constraints;
    constraints.LayoutDirection = winrt::Microsoft::ReactNative::LayoutDirection::Undefined;
    constraints.MaximumSize = constraints.MinimumSize = size;
    rootView.Arrange(constraints, {0, 0});
  }
}

namespace winrt::Microsoft::ReactNative::implementation {
ReactNativeWin32App::ReactNativeWin32App() {}

ReactNativeWin32App::~ReactNativeWin32App() {
  m_desktopChildSiteBridge = nullptr;

  // Destroy all Composition objects
  if (m_compositor != nullptr) {
    m_compositor.Close();
    m_compositor = nullptr;
  }
}

winrt::Microsoft::UI::Windowing::AppWindow ReactNativeWin32App::AppWindow() {
  return m_appWindow;
}

void ReactNativeWin32App::AppWindow(winrt::Microsoft::UI::Windowing::AppWindow const &appWindow) {
  m_appWindow = appWindow;
}

winrt::Microsoft::UI::Composition::Compositor ReactNativeWin32App::Compositor() {
  return m_compositor;
}

void ReactNativeWin32App::Compositor(winrt::Microsoft::UI::Composition::Compositor const &compositor) {
  m_compositor = compositor;
}

void ReactNativeWin32App::DesktopChildSiteBridge(
    winrt::Microsoft::UI::Content::DesktopChildSiteBridge const &desktopChildSiteBridge) {
  m_desktopChildSiteBridge = desktopChildSiteBridge;
}

winrt::Microsoft::UI::Content::DesktopChildSiteBridge ReactNativeWin32App::DesktopChildSiteBridge() {
  return m_desktopChildSiteBridge;
}

winrt::Microsoft::UI::Dispatching::DispatcherQueueController ReactNativeWin32App::DispatcherQueueController() {
  return m_dispatcherQueueController;
}

void ReactNativeWin32App::DispatcherQueueController(
    winrt::Microsoft::UI::Dispatching::DispatcherQueueController const &dispatcherQueueController) {
  m_dispatcherQueueController = dispatcherQueueController;
}

winrt::Microsoft::ReactNative::ReactNativeHost ReactNativeWin32App::ReactNativeHost() {
  if (m_host == nullptr) {
    m_host = winrt::make<winrt::Microsoft::ReactNative::implementation::ReactNativeHost>();
  }
  return m_host;
}

void ReactNativeWin32App::ReactNativeHost(winrt::Microsoft::ReactNative::ReactNativeHost const &host) {
  m_host = host;
}

winrt::Microsoft::ReactNative::ReactNativeIsland ReactNativeWin32App::ReactNativeIsland() {
  return m_reactNativeIsland;
}

void ReactNativeWin32App::ReactNativeIsland(winrt::Microsoft::ReactNative::ReactNativeIsland const &reactNativeIsland) {
  m_reactNativeIsland = reactNativeIsland;
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

void ReactNativeWin32App::Start() {
  // Show the hosting AppWindow
  m_appWindow.Show();

  // Currently set the property to use current thread dispatcher as a default UI dispatcher.
  // TODO: Use the correct dispatcher from a developer-provided DispatcherQueueController
  ReactNativeHost().InstanceSettings().Properties().Set(
      ReactDispatcherHelper::UIDispatcherProperty(), ReactDispatcherHelper::UIThreadDispatcher());

  auto hwnd{winrt::Microsoft::UI::GetWindowFromWindowId(m_appWindow.Id())};

  winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
      ReactNativeHost().InstanceSettings().Properties(), reinterpret_cast<uint64_t>(hwnd));

  winrt::Microsoft::ReactNative::Composition::CompositionUIService::SetCompositor(
      ReactNativeHost().InstanceSettings(), m_compositor);

  // Start the react-native instance, which will create a JavaScript runtime and load the applications bundle.
  ReactNativeHost().ReloadInstance();

  // Create a RootView which will present a react-native component
  if (m_reactNativeIsland == nullptr) {
    m_reactNativeIsland = winrt::Microsoft::ReactNative::ReactNativeIsland(m_compositor);
  }

  m_reactNativeIsland.ReactViewHost(
      winrt::Microsoft::ReactNative::ReactCoreInjection::MakeViewHost(ReactNativeHost(), ReactViewOptions()));

  // Update the size of the RootView when the AppWindow changes size
  m_appWindow.Changed([wkRootView = winrt::make_weak(m_reactNativeIsland)](
                          winrt::Microsoft::UI::Windowing::AppWindow const &window,
                          winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs const &args) {
    if (args.DidSizeChange() || args.DidVisibilityChange()) {
      if (auto rootView = wkRootView.get()) {
        UpdateRootViewSizeToAppWindow(rootView, window);
      }
    }
  });

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

  // DesktopChildSiteBridge create a ContentSite that can host the RootView ContentIsland
  m_desktopChildSiteBridge =
      winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(m_compositor, m_appWindow.Id());

  m_desktopChildSiteBridge.Connect(m_reactNativeIsland.Island());

  m_desktopChildSiteBridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);

  auto scaleFactor = ScaleFactor(hwnd);
  m_reactNativeIsland.ScaleFactor(scaleFactor);

  UpdateRootViewSizeToAppWindow(m_reactNativeIsland, m_appWindow);

  m_desktopChildSiteBridge.Show();

  // Run the main application event loop
  m_dispatcherQueueController.DispatcherQueue().RunEventLoop();

  // Rundown the DispatcherQueue. This drains the queue and raises events to let components
  // know the message loop has finished.
  m_dispatcherQueueController.ShutdownQueue();

  m_desktopChildSiteBridge.Close();
}
} // namespace winrt::Microsoft::ReactNative::implementation
