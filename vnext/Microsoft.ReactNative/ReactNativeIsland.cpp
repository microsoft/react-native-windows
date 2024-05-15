#include "pch.h"
#include "ReactNativeIsland.h"
#include "ReactNativeIsland.g.cpp"
#include <winrt/Microsoft.UI.Interop.h>

namespace winrt::ReactNative
{
    using namespace winrt::Microsoft::ReactNative;
    using namespace winrt::Microsoft::ReactNative::Composition;
}

namespace winrt::UI
{
    using namespace winrt::Microsoft::UI::Composition;
    using namespace winrt::Microsoft::UI::Content;
    using namespace winrt::Microsoft::UI::Input;
    using namespace winrt::Microsoft::UI::Windowing;
}

float ScaleFactor(HWND hwnd) noexcept {
    return GetDpiForWindow(hwnd) / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
}

namespace winrt::Microsoft::ReactNative::implementation
{
    winrt::ReactNative::ReactNativeIsland ReactNativeIsland::Create(
        winrt::UI::Compositor const& compositor,
        winrt::UI::AppWindow const& window,
        winrt::ReactNative::ReactNativeHost const& host,
        winrt::hstring const& mainComponentName)
    {
        return winrt::make<ReactNativeIsland>(compositor, window, host, mainComponentName);
    }

    ReactNativeIsland::~ReactNativeIsland()
    {
        m_island.Close();
    }

    void ReactNativeIsland::ApplyScaleFactor(winrt::UI::AppWindow const& window)
    {
        auto hwnd = winrt::Microsoft::UI::GetWindowFromWindowId(window.Id());
        m_scaleFactor = ScaleFactor(hwnd);
    }

    ReactNativeIsland::ReactNativeIsland(
        winrt::UI::Compositor const& compositor,
        winrt::UI::AppWindow const& window,
        winrt::ReactNative::ReactNativeHost const& host,
        winrt::hstring const& mainComponentName)
        : m_compositor(compositor)
    {
        ApplyScaleFactor(window);

        winrt::ReactNative::ReactViewOptions viewOptions;
        viewOptions.ComponentName(mainComponentName);

        // Create a RootView (creates a spriteVisual) and associates it with an island
        m_rootView = winrt::ReactNative::CompositionRootView(m_compositor);
        m_rootView.ReactViewHost(
            winrt::ReactNative::ReactCoreInjection::MakeViewHost(host, viewOptions));

        // Create a contentIsland
        m_island = m_rootView.Island();

        auto invScale = 1.0f / m_scaleFactor;
        m_rootView.RootVisual().Scale({ invScale, invScale, invScale });
        m_rootView.ScaleFactor(m_scaleFactor);

        // Set the intialSize of the root view
        UpdateRootViewSizeToAppWindow(window);

        // Lifted input base setup.
        m_mouseInput = winrt::UI::InputPointerSource::GetForIsland(m_island);
        m_keyboardInput = winrt::UI::InputKeyboardSource::GetForIsland(m_island);
    }

    void ReactNativeIsland::InitalizeInputHandlers()
    {
        throw winrt::hresult_not_implemented();
    }

    void ReactNativeIsland::UpdateRootViewSizeToAppWindow(
        winrt::Microsoft::UI::Windowing::AppWindow const& window) {

        winrt::Windows::Foundation::Size size{
            window.ClientSize().Width / m_scaleFactor, window.ClientSize().Height / m_scaleFactor };

        // Do not relayout when minimized
        if (window.Presenter().as<winrt::UI::OverlappedPresenter>().State() !=
            winrt::UI::OverlappedPresenterState::Minimized) {
            m_rootView.Arrange(size);
            m_rootView.Size(size);
        }
    }
}
