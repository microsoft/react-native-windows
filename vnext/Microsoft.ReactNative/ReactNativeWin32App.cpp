#include "pch.h"
#include "ReactNativeWin32App.h"
#include "ReactNativeWin32App.g.cpp"
#include "ReactNativeHost.h"
#include "winrt/Microsoft.UI.Composition.h"
#include "winrt/Microsoft.UI.Content.h"
#include "winrt/Microsoft.UI.Interop.h"
#include "winrt/Microsoft.UI.Windowing.h"
#include "winrt/Microsoft.UI.Dispatching.h"

namespace winrt::Microsoft::ReactNative::implementation {
ReactNativeWin32App::ReactNativeWin32App() {}

ReactNativeWin32App::~ReactNativeWin32App() {
  m_desktopChildSiteBridge = nullptr;

  // Destroy all Composition objects
  m_compositor.Close();
  m_compositor = nullptr;
}

void ReactNativeWin32App::AppWindow(winrt::Microsoft::UI::Windowing::AppWindow const &appWindow) {
  m_appWindow = appWindow;
}

winrt::Microsoft::UI::Windowing::AppWindow ReactNativeWin32App::AppWindow() {
  return m_appWindow;
}

void ReactNativeWin32App::Compositor(winrt::Microsoft::UI::Composition::Compositor const &compositor) {
  m_compositor = compositor;
}

winrt::Microsoft::UI::Composition::Compositor ReactNativeWin32App::Compositor() {
  return m_compositor;
}

winrt::Microsoft::ReactNative::ReactNativeIsland ReactNativeWin32App::ReactNativeIsland() {
  return m_reactNativeIsland;
}

void ReactNativeWin32App::ReactNativeIsland(winrt::Microsoft::ReactNative::ReactNativeIsland const &reactNativeIsland) {
  m_reactNativeIsland = reactNativeIsland;
}

winrt::Microsoft::UI::Content::DesktopChildSiteBridge ReactNativeWin32App::DesktopChildSiteBridge() {
  return m_desktopChildSiteBridge;
}

void ReactNativeWin32App::DesktopChildSiteBridge(
    winrt::Microsoft::UI::Content::DesktopChildSiteBridge const &desktopChildSiteBridge) {
  m_desktopChildSiteBridge = desktopChildSiteBridge;
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

void ReactNativeWin32App::DispatchQueueController(
    winrt::Microsoft::UI::Dispatching::DispatcherQueueController const &dispatcherQueueController) {
  m_dispatcherQueueController = dispatcherQueueController;
}

void ReactNativeWin32App::Start() {
  m_desktopChildSiteBridge.Show();

  // Run the main application event loop
  m_dispatcherQueueController.DispatcherQueue().RunEventLoop();

  // Rundown the DispatcherQueue. This drains the queue and raises events to let components
  // know the message loop has finished.
  m_dispatcherQueueController.ShutdownQueue();

  m_desktopChildSiteBridge.Close();
}
} // namespace winrt::Microsoft::ReactNative::implementation
