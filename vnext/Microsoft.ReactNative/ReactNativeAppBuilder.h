#pragma once
#include "ReactNativeAppBuilder.g.h"

namespace winrt::Microsoft::ReactNative::implementation
{
    struct ReactNativeAppBuilder : ReactNativeAppBuilderT<ReactNativeAppBuilder>
    {
        ReactNativeAppBuilder();

        winrt::Microsoft::ReactNative::ReactNativeAppBuilder AddPackageProviders(winrt::Windows::Foundation::Collections::IVector<winrt::Microsoft::ReactNative::IReactPackageProvider> const& packageProviders);
        winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetReactInstanceSettings(winrt::Microsoft::ReactNative::ReactInstanceSettings const& settings);
        winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetCompositor(winrt::Microsoft::UI::Composition::Compositor const& compositor);
        winrt::Microsoft::ReactNative::ReactNativeAppBuilder SetAppWindow(winrt::Microsoft::UI::Windowing::AppWindow const& appWindow);
        winrt::Microsoft::ReactNative::ReactNativeHost Start();

    private:
        winrt::Microsoft::UI::Windowing::AppWindow m_appWindow{nullptr};
        winrt::Microsoft::UI::Composition::Compositor m_compositor{};
        winrt::Microsoft::ReactNative::ReactNativeHost m_host{};
    };
}
namespace winrt::Microsoft::ReactNative::factory_implementation
{
    struct ReactNativeAppBuilder : ReactNativeAppBuilderT<ReactNativeAppBuilder, implementation::ReactNativeAppBuilder>
    {
    };
}
