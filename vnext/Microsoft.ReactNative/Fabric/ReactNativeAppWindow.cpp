// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "ReactNativeAppWindow.h"
#include "ReactNativeAppWindow.g.cpp"

namespace winrt::Microsoft::ReactNative::implementation
{
    ReactNativeAppWindow::ReactNativeAppWindow(winrt::Microsoft::UI::Windowing::AppWindow const& appWindow)
    {
        m_appWindow = appWindow;
    }

    winrt::Microsoft::ReactNative::ReactNativeAppWindow ReactNativeAppWindow::Create()
    {
        auto appWindow{winrt::Microsoft::UI::Windowing::AppWindow::Create()};
        return winrt::make<ReactNativeAppWindow>(appWindow);
    }

    winrt::Microsoft::ReactNative::ReactNativeAppWindow ReactNativeAppWindow::Create(
        winrt::Microsoft::UI::Windowing::AppWindowPresenter const& appWindowPresenter)
    {
        auto appWindow{winrt::Microsoft::UI::Windowing::AppWindow::Create(appWindowPresenter)};
        return winrt::make<ReactNativeAppWindow>(appWindow);
    }

    winrt::Microsoft::ReactNative::ReactNativeAppWindow ReactNativeAppWindow::Create(
        winrt::Microsoft::UI::Windowing::AppWindowPresenter const& appWindowPresenter,
        winrt::Microsoft::UI::WindowId const& ownerWindowId)
    {
        auto appWindow{winrt::Microsoft::UI::Windowing::AppWindow::Create(appWindowPresenter, ownerWindowId)};
        return winrt::make<ReactNativeAppWindow>(appWindow);
    }

    winrt::Microsoft::ReactNative::ReactNativeAppWindow ReactNativeAppWindow::Create(
        winrt::Microsoft::UI::Windowing::AppWindowPresenter const& appWindowPresenter,
        winrt::Microsoft::UI::WindowId const& ownerWindowId,
        winrt::Microsoft::UI::Dispatching::DispatcherQueue const& dispatcherQueue)
    {
        auto appWindow{winrt::Microsoft::UI::Windowing::AppWindow::Create(appWindowPresenter, ownerWindowId, dispatcherQueue)};
        return winrt::make<ReactNativeAppWindow>(appWindow);
    }

    winrt::Microsoft::ReactNative::ReactNativeAppWindow ReactNativeAppWindow::GetFromWindowId(winrt::Microsoft::UI::WindowId const& windowId)
    {
        auto appWindow{winrt::Microsoft::UI::Windowing::AppWindow::GetFromWindowId(windowId)};
        return winrt::make<ReactNativeAppWindow>(appWindow);
    }

    winrt::Windows::Graphics::SizeInt32 ReactNativeAppWindow::ClientSize()
    {
        return m_appWindow.ClientSize();
    }

    winrt::Microsoft::UI::Dispatching::DispatcherQueue ReactNativeAppWindow::DispatcherQueue()
    {
        return m_appWindow.DispatcherQueue();
    }

    winrt::Microsoft::UI::WindowId ReactNativeAppWindow::Id()
    {
        return m_appWindow.Id();
    }

    bool ReactNativeAppWindow::IsShownInSwitchers()
    {
        return m_appWindow.IsShownInSwitchers();
    }

    void ReactNativeAppWindow::IsShownInSwitchers(bool value)
    {
        m_appWindow.IsShownInSwitchers(value);
    }

    bool ReactNativeAppWindow::IsVisible()
    {
        return m_appWindow.IsVisible();
    }

    winrt::Microsoft::UI::WindowId ReactNativeAppWindow::OwnerWindowId()
    {
        return m_appWindow.OwnerWindowId();
    }

    winrt::Windows::Graphics::PointInt32 ReactNativeAppWindow::Position()
    {
        return m_appWindow.Position();
    }

    winrt::Microsoft::UI::Windowing::AppWindowPresenter ReactNativeAppWindow::Presenter()
    {
        return m_appWindow.Presenter();
    }

    winrt::Windows::Graphics::SizeInt32 ReactNativeAppWindow::Size()
    {
        return m_appWindow.Size();
    }

    hstring ReactNativeAppWindow::Title()
    {
        return m_appWindow.Title();
    }

    void ReactNativeAppWindow::Title(hstring const& value)
    {
        m_appWindow.Title(value);
    }

    winrt::Microsoft::UI::Windowing::AppWindowTitleBar ReactNativeAppWindow::TitleBar()
    {
        return m_appWindow.TitleBar();
    }

    void ReactNativeAppWindow::AssociateWithDispatcherQueue(winrt::Microsoft::UI::Dispatching::DispatcherQueue const& dispatcherQueue)
    {
        m_appWindow.AssociateWithDispatcherQueue(dispatcherQueue);
    }

    void ReactNativeAppWindow::Destroy()
    {
        m_appWindow.Destroy();
    }

    void ReactNativeAppWindow::Hide()
    {
        m_appWindow.Hide();
    }

    void ReactNativeAppWindow::Move(winrt::Windows::Graphics::PointInt32 const& position)
    {
        m_appWindow.Move(position);
    }

    void ReactNativeAppWindow::MoveAndResize(winrt::Windows::Graphics::RectInt32 const& rect)
    {
        m_appWindow.MoveAndResize(rect);
    }

    void ReactNativeAppWindow::MoveAndResize(winrt::Windows::Graphics::RectInt32 const& rect, winrt::Microsoft::UI::Windowing::DisplayArea const& displayarea)
    {
        m_appWindow.MoveAndResize(rect, displayarea);
    }

    void ReactNativeAppWindow::MoveInZOrderAtBottom()
    {
        m_appWindow.MoveInZOrderAtBottom();
    }

    void ReactNativeAppWindow::MoveInZOrderAtTop()
    {
        m_appWindow.MoveInZOrderAtTop();
    }

    void ReactNativeAppWindow::MoveInZOrderBelow(winrt::Microsoft::UI::WindowId const& windowId)
    {
        m_appWindow.MoveInZOrderBelow(windowId);
    }

    void ReactNativeAppWindow::Resize(winrt::Windows::Graphics::SizeInt32 const& size)
    {
        m_appWindow.Resize(size);
    }

    void ReactNativeAppWindow::ResizeClient(winrt::Windows::Graphics::SizeInt32 const& size)
    {
        m_appWindow.ResizeClient(size);
    }

    void ReactNativeAppWindow::SetIcon(hstring const& iconPath)
    {
        m_appWindow.SetIcon(iconPath);
    }

    void ReactNativeAppWindow::SetIcon(winrt::Microsoft::UI::IconId const& iconId)
    {
        m_appWindow.SetIcon(iconId);
    }

    void ReactNativeAppWindow::SetPresenter(winrt::Microsoft::UI::Windowing::AppWindowPresenter const& appWindowPresenter)
    {
        m_appWindow.SetPresenter(appWindowPresenter);
    }

    void ReactNativeAppWindow::SetPresenter(winrt::Microsoft::UI::Windowing::AppWindowPresenterKind const& appWindowPresenterKind)
    {
        m_appWindow.SetPresenter(appWindowPresenterKind);
    }

    void ReactNativeAppWindow::Show()
    {
        m_appWindow.Show();
    }

    void ReactNativeAppWindow::Show(bool activateWindow)
    {
        m_appWindow.Show(activateWindow);
    }

    void ReactNativeAppWindow::ShowOnceWithRequestedStartupState()
    {
        m_appWindow.ShowOnceWithRequestedStartupState();
    }

    winrt::event_token ReactNativeAppWindow::Changed(winrt::Windows::Foundation::TypedEventHandler<winrt::Microsoft::ReactNative::ReactNativeAppWindow, winrt::Microsoft::UI::Windowing::AppWindowChangedEventArgs> const& handler)
    {
        // TO BE IMPLEMENTED
        throw hresult_not_implemented();
    }

    void ReactNativeAppWindow::Changed(winrt::event_token const& token) noexcept
    {
        // TO BE IMPLEMENTED
    }

    winrt::event_token ReactNativeAppWindow::Closing(winrt::Windows::Foundation::TypedEventHandler<winrt::Microsoft::ReactNative::ReactNativeAppWindow, winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs> const& handler)
    {
        // TO BE IMPLEMENTED
        throw hresult_not_implemented();
    }

    void ReactNativeAppWindow::Closing(winrt::event_token const& token) noexcept
    {
        // TO BE IMPLEMENTED
    }

    winrt::event_token ReactNativeAppWindow::Destroying(winrt::Windows::Foundation::TypedEventHandler<winrt::Microsoft::ReactNative::ReactNativeAppWindow, winrt::Windows::Foundation::IInspectable> const& handler)
    {
        // TO BE IMPLEMENTED
        throw hresult_not_implemented();
    }

    void ReactNativeAppWindow::Destroying(winrt::event_token const& token) noexcept
    {
        // TO BE IMPLEMENTED
    }
}
