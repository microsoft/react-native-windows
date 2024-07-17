#pragma once
#include "ReactNativeAppBuilder.g.h"
#include <winrt/Microsoft.UI.Content.h>

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactNativeAppBuilder : ReactNativeAppBuilderT<ReactNativeAppBuilder> {
  ReactNativeAppBuilder();

  ~ReactNativeAppBuilder();

  // Properties
  winrt::Microsoft::UI::Content::DesktopChildSiteBridge DesktopChildSiteBridge();

  winrt::Microsoft::ReactNative::ReactNativeIsland ReactNativeIsland();

  winrt::Microsoft::ReactNative::ReactNativeAppBuilder AddPackageProviders(
      winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::ReactNative::IReactPackageProvider> const
          &packageProviders);
  winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetReactInstanceSettings(
      winrt::Microsoft::ReactNative::ReactInstanceSettings const &settings);
  winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetCompositor(
      winrt::Microsoft::UI::Composition::Compositor const &compositor);
  winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetAppWindow(
      winrt::Microsoft::UI::Windowing::AppWindow const &appWindow);
  winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetReactViewOptions(
      winrt::Microsoft::ReactNative::ReactViewOptions const &reactViewOptions);
  winrt::Microsoft::ReactNative::ReactNativeHost Start();

 private:
  winrt::Microsoft::UI::Windowing::AppWindow m_appWindow{nullptr};
  winrt::Microsoft::UI::Composition::Compositor m_compositor{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeHost m_host{};
  winrt::Microsoft::ReactNative::ReactViewOptions m_reactViewOptions{};
  winrt::Microsoft::UI::Content::DesktopChildSiteBridge m_desktopChildSiteBridge{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeIsland m_reactNativeIsland{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::implementation
namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactNativeAppBuilder : ReactNativeAppBuilderT<ReactNativeAppBuilder, implementation::ReactNativeAppBuilder> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
