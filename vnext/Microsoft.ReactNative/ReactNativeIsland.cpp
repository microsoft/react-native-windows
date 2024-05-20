#include "pch.h"
#include "ReactNativeIsland.h"
#include "ReactNativeIsland.g.cpp"

namespace winrt::ReactNative {
using namespace winrt::Microsoft::ReactNative;
} // namespace winrt::ReactNative

namespace winrt::UI {
using namespace winrt::Microsoft::UI;
} // namespace winrt::UI

namespace winrt::Microsoft::ReactNative::implementation {
ReactNativeIsland::ReactNativeIsland(
    winrt::UI::Composition::Compositor const &compositor,
    winrt::UI::Windowing::AppWindow const &window,
    winrt::ReactNative::ReactNativeHost const &host,
    winrt::hstring const &mainComponentName)
    : m_compositor(compositor) {
  winrt::ReactNative::ReactViewOptions viewOptions;
  viewOptions.ComponentName(mainComponentName);

  // Create a RootView (creates a spriteVisual) and associates it with a ContentIsland
  m_rootView = winrt::ReactNative::CompositionRootView(m_compositor);
  m_rootView.ReactViewHost(winrt::ReactNative::ReactCoreInjection::MakeViewHost(host, viewOptions));
}
} // namespace winrt::Microsoft::ReactNative::implementation
