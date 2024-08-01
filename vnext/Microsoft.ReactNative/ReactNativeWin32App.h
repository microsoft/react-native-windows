#pragma once
#include "ReactNativeWin32App.g.h"
#include "winrt/Microsoft.UI.Dispatching.h"

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactNativeWin32App : ReactNativeWin32AppT<ReactNativeWin32App> {
  ReactNativeWin32App();
  ~ReactNativeWin32App();

  void AppWindow(winrt::Microsoft::UI::Windowing::AppWindow const &appWindow);
  winrt::Microsoft::UI::Windowing::AppWindow AppWindow();

  winrt::Microsoft::UI::Composition::Compositor Compositor();
  void Compositor(winrt::Microsoft::UI::Composition::Compositor const &compositor);

  winrt::Microsoft::ReactNative::ReactNativeIsland ReactNativeIsland();
  void ReactNativeIsland(winrt::Microsoft::ReactNative::ReactNativeIsland const &reactNativeIsland);

  winrt::Microsoft::UI::Content::DesktopChildSiteBridge DesktopChildSiteBridge();
  void DesktopChildSiteBridge(winrt::Microsoft::UI::Content::DesktopChildSiteBridge const &desktopChildSiteBridge);

  void DispatchQueueController(
      winrt::Microsoft::UI::Dispatching::DispatcherQueueController const &dispatcherQueueController);

  winrt::Microsoft::ReactNative::ReactNativeHost ReactNativeHost();
  void ReactNativeHost(winrt::Microsoft::ReactNative::ReactNativeHost const &host);

  void Start();

 private:
  winrt::Microsoft::UI::Windowing::AppWindow m_appWindow{nullptr};
  winrt::Microsoft::UI::Composition::Compositor m_compositor{nullptr};
  winrt::Microsoft::UI::Dispatching::DispatcherQueueController m_dispatcherQueueController{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeHost m_host{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeIsland m_reactNativeIsland{nullptr};
  winrt::Microsoft::UI::Content::DesktopChildSiteBridge m_desktopChildSiteBridge{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::implementation
