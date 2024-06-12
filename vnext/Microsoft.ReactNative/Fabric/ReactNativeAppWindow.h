// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "ReactNativeAppWindow.g.h"

namespace winrt::Microsoft::ReactNative::implementation
{
    struct ReactNativeAppWindow : ReactNativeAppWindowT<ReactNativeAppWindow>
    {
        // Constructors
        ReactNativeAppWindow() = default;
        ReactNativeAppWindow(winrt::Microsoft::UI::Windowing::AppWindow const& appWindow);

        // Properties
        winrt::Windows::Graphics::SizeInt32 ClientSize();
        winrt::Microsoft::UI::Dispatching::DispatcherQueue DispatcherQueue();
        winrt::Microsoft::UI::WindowId Id();
        bool IsShownInSwitchers();
        void IsShownInSwitchers(bool value);
        bool IsVisible();
        winrt::Microsoft::UI::WindowId OwnerWindowId();
        winrt::Windows::Graphics::PointInt32 Position();
        winrt::Microsoft::UI::Windowing::AppWindowPresenter Presenter();
        winrt::Windows::Graphics::SizeInt32 Size();
        hstring Title();
        void Title(hstring const& value);
        winrt::Microsoft::UI::Windowing::AppWindowTitleBar TitleBar();

        // Methods
        void AssociateWithDispatcherQueue(winrt::Microsoft::UI::Dispatching::DispatcherQueue const& dispatcherQueue);
        static winrt::Microsoft::ReactNative::ReactNativeAppWindow Create();
        static winrt::Microsoft::ReactNative::ReactNativeAppWindow Create(winrt::Microsoft::UI::Windowing::AppWindowPresenter const& appWindowPresenter);
        static winrt::Microsoft::ReactNative::ReactNativeAppWindow Create(winrt::Microsoft::UI::Windowing::AppWindowPresenter const& appWindowPresenter, winrt::Microsoft::UI::WindowId const& ownerWindowId);
        static winrt::Microsoft::ReactNative::ReactNativeAppWindow Create(winrt::Microsoft::UI::Windowing::AppWindowPresenter const& appWindowPresenter, winrt::Microsoft::UI::WindowId const& ownerWindowId, winrt::Microsoft::UI::Dispatching::DispatcherQueue const& dispatcherQueue);
        static winrt::Microsoft::ReactNative::ReactNativeAppWindow GetFromWindowId(winrt::Microsoft::UI::WindowId const& windowId);
        void Destroy();
        void Hide();
        void Move(winrt::Windows::Graphics::PointInt32 const& position);
        void MoveAndResize(winrt::Windows::Graphics::RectInt32 const& rect);
        void MoveAndResize(winrt::Windows::Graphics::RectInt32 const& rect, winrt::Microsoft::UI::Windowing::DisplayArea const& displayarea);
        void MoveInZOrderAtBottom();
        void MoveInZOrderAtTop();
        void MoveInZOrderBelow(winrt::Microsoft::UI::WindowId const& windowId);
        void Resize(winrt::Windows::Graphics::SizeInt32 const& size);
        void ResizeClient(winrt::Windows::Graphics::SizeInt32 const& size);
        void SetIcon(hstring const& iconPath);
        void SetIcon(winrt::Microsoft::UI::IconId const& iconId);
        void SetPresenter(winrt::Microsoft::UI::Windowing::AppWindowPresenter const& appWindowPresenter);
        void SetPresenter(winrt::Microsoft::UI::Windowing::AppWindowPresenterKind const& appWindowPresenterKind);
        void Show();
        void Show(bool activateWindow);
        void ShowOnceWithRequestedStartupState();

        // Events
        winrt::event_token Changed(winrt::Windows::Foundation::TypedEventHandler<winrt::Microsoft::ReactNative::ReactNativeAppWindow, winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs> const& handler);
        void Changed(winrt::event_token const& token) noexcept;
        winrt::event_token Closing(winrt::Windows::Foundation::TypedEventHandler<winrt::Microsoft::ReactNative::ReactNativeAppWindow, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs> const& handler);
        void Closing(winrt::event_token const& token) noexcept;
        winrt::event_token Destroying(winrt::Windows::Foundation::TypedEventHandler<winrt::Microsoft::ReactNative::ReactNativeAppWindow, winrt::Windows::Foundation::IInspectable> const& handler);
        void Destroying(winrt::event_token const& token) noexcept;

    private:
        winrt::Microsoft::UI::Windowing::AppWindow m_appWindow{nullptr};
    };
}
namespace winrt::Microsoft::ReactNative::factory_implementation
{
    struct ReactNativeAppWindow : ReactNativeAppWindowT<ReactNativeAppWindow, implementation::ReactNativeAppWindow>
    {
    };
}
