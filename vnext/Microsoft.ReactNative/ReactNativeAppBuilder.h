#pragma once
#include "ReactNativeAppBuilder.g.h"
#include <winrt/Microsoft.UI.Content.h>

namespace winrt::Microsoft::ReactNative::implementation {
struct ReactNativeAppBuilder : ReactNativeAppBuilderT<ReactNativeAppBuilder> {
  ReactNativeAppBuilder();

  ~ReactNativeAppBuilder();

  winrt::Microsoft::ReactNative::ReactNativeAppBuilder AddPackageProviders(
      winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::ReactNative::IReactPackageProvider> const
          &packageProviders);
  winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetReactInstanceSettings(
      winrt::Microsoft::ReactNative::ReactInstanceSettings const &settings);

  // TODO: Currently, SetCompositor API is not exposed to the developer.
  // Compositor depends on the DispatcherQueue created by DispatcherQueueController on a current thread
  // or dedicated thread. So we also have to make a provision for setting DispatcherQueueController.
  winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetCompositor(
      winrt::Microsoft::UI::Composition::Compositor const &compositor);
  winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetAppWindow(
      winrt::Microsoft::UI::Windowing::AppWindow const &appWindow);
  winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetReactViewOptions(
      winrt::Microsoft::ReactNative::ReactViewOptions const &reactViewOptions);
  winrt::Microsoft::ReactNative::ReactNativeWin32App Build();

 private:
  winrt::Microsoft::ReactNative::ReactViewOptions m_reactViewOptions{};
  winrt::Microsoft::ReactNative::ReactNativeWin32App m_reactNativeWin32App{nullptr};
};
} // namespace winrt::Microsoft::ReactNative::implementation
namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactNativeAppBuilder : ReactNativeAppBuilderT<ReactNativeAppBuilder, implementation::ReactNativeAppBuilder> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
