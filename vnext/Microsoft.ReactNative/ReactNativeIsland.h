#pragma once
#include "ReactNativeIsland.g.h"

namespace winrt::Microsoft::ReactNative::implementation
{
    struct ReactNativeIsland : ReactNativeIslandT<ReactNativeIsland>
    {
        ReactNativeIsland() = default;

        ReactNativeIsland(
            const winrt::Microsoft::UI::Composition::Compositor& compositor,
            winrt::Microsoft::UI::Windowing::AppWindow const& window,
            winrt::Microsoft::ReactNative::ReactNativeHost const& host,
            winrt::hstring const& mainComponentName);

        ~ReactNativeIsland();

        static winrt::Microsoft::ReactNative::ReactNativeIsland Create(
            winrt::Microsoft::UI::Composition::Compositor const& compositor,
            winrt::Microsoft::UI::Windowing::AppWindow const& window,
            winrt::Microsoft::ReactNative::ReactNativeHost const& host,
            winrt::hstring const& mainComponentName);

        winrt::Microsoft::UI::Content::ContentIsland Island() const
        {
            return m_island;
        }

        void UpdateRootViewSizeToAppWindow(
            winrt::Microsoft::UI::Windowing::AppWindow const& window);

	  private:
        void ApplyScaleFactor(winrt::Microsoft::UI::Windowing::AppWindow const& window);

        // Initialize Mouse and Keyboard input handlers
        void InitalizeInputHandlers();

        winrt::Microsoft::UI::Composition::Compositor m_compositor{nullptr};
        winrt::Microsoft::ReactNative::CompositionRootView m_rootView { nullptr };
        winrt::Microsoft::UI::Content::ContentIsland m_island{ nullptr };

        float m_scaleFactor{ 0 };

        // Input objects.
        winrt::Microsoft::UI::Input::InputPointerSource m_mouseInput{ nullptr };
        winrt::Microsoft::UI::Input::InputKeyboardSource m_keyboardInput{ nullptr };
    };
}
namespace winrt::Microsoft::ReactNative::factory_implementation
{
    struct ReactNativeIsland : ReactNativeIslandT<ReactNativeIsland, implementation::ReactNativeIsland>
    {
    };
}
