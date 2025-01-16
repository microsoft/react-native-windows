// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsModalHostViewComponentView.h"

#include "../../../codegen/react/components/rnwcore/ModalHostView.g.h"
#include <ComponentView.Experimental.interop.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.interop.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct ModalHostView : public winrt::implements<ModalHostView, winrt::Windows::Foundation::IInspectable>,
                       ::Microsoft::ReactNativeSpecs::BaseModalHostView<ModalHostView> {
  ~ModalHostView() {
    if (m_window && m_window.IsVisible()) {
      CloseWindow();
    }

    if (m_reactNativeIsland) {
      m_reactNativeIsland.Island().Close();
    }

    if (m_bridge) {
      if (m_departFocusToken && !m_bridge.IsClosed()) {
        auto navHost = winrt::Microsoft::UI::Input::InputFocusNavigationHost::GetForSiteBridge(m_bridge);
        navHost.DepartFocusRequested(m_departFocusToken);
      }
      m_bridge.Close();
    }

    if (m_window) {
      m_window.Destroy();
      m_window = nullptr;
    }

#ifdef USE_EXPERIMENTAL_WINUI3
    if (m_popUp) {
      m_popUp.Close();
      m_popUp = nullptr;
    }
#endif
  }

  void InitializePortalViewComponent(
      const winrt::Microsoft::ReactNative::Composition::PortalComponentView &portalComponentView) noexcept {
    m_reactContext = portalComponentView.ReactContext();

    portalComponentView.Mounted(
        [](const auto & /*sender*/, const auto &view) { view.UserData().as<ModalHostView>()->OnMounted(view); });
    portalComponentView.Unmounted(
        [](const auto & /*sender*/, const auto &view) { view.UserData().as<ModalHostView>()->OnUnmounted(view); });
  }

  void UpdateProps(
      const winrt::Microsoft::ReactNative::ComponentView &view,
      const winrt::com_ptr<::Microsoft::ReactNativeSpecs::ModalHostViewProps> &newProps,
      const winrt::com_ptr<::Microsoft::ReactNativeSpecs::ModalHostViewProps> &oldProps) noexcept override {
    if (!oldProps || newProps->visible != oldProps->visible) {
      if (newProps->visible.value_or(true)) {
        // We do not immediately show the window, since we want to resize/position
        // the window based on the layout metrics before we show it
        m_showQueued = true;
      } else {
        CloseWindow();
      }
    }
    ::Microsoft::ReactNativeSpecs::BaseModalHostView<ModalHostView>::UpdateProps(view, newProps, oldProps);
  }

  void UpdateLayoutMetrics(
      const winrt::Microsoft::ReactNative::ComponentView &view,
      const winrt::Microsoft::ReactNative::LayoutMetrics &newLayoutMetrics,
      const winrt::Microsoft::ReactNative::LayoutMetrics & /*oldLayoutMetrics*/) noexcept override {
    if (m_window) {
      AdjustWindowSize(newLayoutMetrics);
    }
  }

  void FinalizeUpdate(
      const winrt::Microsoft::ReactNative::ComponentView &view,
      winrt::Microsoft::ReactNative::ComponentViewUpdateMask /*mask*/) noexcept override {
    if (m_showQueued) {
      ShowOnUIThread(view);
    }
  }

 private:
  void OnMounted(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
    m_mounted = true;
    if (m_showQueued) {
      ShowOnUIThread(view);
    }
  }

  void OnUnmounted(const winrt::Microsoft::ReactNative::ComponentView & /*view*/) noexcept {
    m_mounted = false;
  }

  void AdjustWindowSize(const winrt::Microsoft::ReactNative::LayoutMetrics &layoutMetrics) noexcept {
    if (layoutMetrics.Frame.Width == 0 && layoutMetrics.Frame.Height == 0) {
      return;
    }

    // get Modal's position based on parent
    RECT parentRC;
    GetWindowRect(m_parentHwnd, &parentRC);
    int32_t xCor = static_cast<int32_t>(
        (parentRC.left + parentRC.right - layoutMetrics.Frame.Width * layoutMetrics.PointScaleFactor) / 2);
    int32_t yCor = static_cast<int32_t>(
        (parentRC.top + parentRC.bottom - layoutMetrics.Frame.Height * layoutMetrics.PointScaleFactor) / 2);

#ifdef USE_EXPERIMENTAL_WINUI3
    winrt::Windows::Graphics::RectInt32 rect2{
        (int)xCor,
        (int)yCor,
        static_cast<int32_t>(layoutMetrics.Frame.Width * (layoutMetrics.PointScaleFactor)),
        static_cast<int32_t>(layoutMetrics.Frame.Height * (layoutMetrics.PointScaleFactor))};
    m_popUp.MoveAndResize(rect2);
#else
    // Adjust window position and size
    m_window.ResizeClient(
        {static_cast<int32_t>(layoutMetrics.Frame.Width * (layoutMetrics.PointScaleFactor)),
         static_cast<int32_t>(layoutMetrics.Frame.Height * (layoutMetrics.PointScaleFactor))});
    m_window.Move({xCor, yCor});
#endif
  };

  void ShowOnUIThread(const winrt::Microsoft::ReactNative::ComponentView &view) {
    if (!m_mounted)
      return;

    m_showQueued = false;
    EnsureModalCreated(view);

#ifdef USE_EXPERIMENTAL_WINUI3
    if (m_popUp) {
      m_bridge.Enable();
      m_popUp.Show();

      auto navHost = winrt::Microsoft::UI::Input::InputFocusNavigationHost::GetForSiteBridge(
          m_popUp.as<winrt::Microsoft::UI::Content::IContentSiteBridge>());
      auto result = navHost.NavigateFocus(winrt::Microsoft::UI::Input::FocusNavigationRequest::Create(
          winrt::Microsoft::UI::Input::FocusNavigationReason::First));

      // dispatch onShow event
      if (auto eventEmitter = EventEmitter()) {
        ::Microsoft::ReactNativeSpecs::ModalHostViewEventEmitter::OnShow eventArgs;
        eventEmitter->onShow(eventArgs);
      }
    }
#endif

    if (m_window && !m_window.IsVisible()) {
      m_bridge.Enable();
      m_window.Show(true);

      auto navHost = winrt::Microsoft::UI::Input::InputFocusNavigationHost::GetForSiteBridge(m_bridge);
      auto result = navHost.NavigateFocus(winrt::Microsoft::UI::Input::FocusNavigationRequest::Create(
          winrt::Microsoft::UI::Input::FocusNavigationReason::First));

      // dispatch onShow event
      if (auto eventEmitter = EventEmitter()) {
        ::Microsoft::ReactNativeSpecs::ModalHostViewEventEmitter::OnShow eventArgs;
        eventEmitter->onShow(eventArgs);
      }
    }
  }

  void CloseWindow() noexcept {
    // enable input to parent before closing the modal window, so focus can return back to the parent window
    EnableWindow(m_parentHwnd, true);

    if (m_window) {
      m_window.Hide();
    }

#ifdef USE_EXPERIMENTAL_WINUI3
    if (m_popUp) {
      m_popUp.Hide();
    }
#endif

    // dispatch onDismiss event
    if (auto eventEmitter = EventEmitter()) {
      ::Microsoft::ReactNativeSpecs::ModalHostViewEventEmitter::OnDismiss eventArgs;
      eventEmitter->onDismiss(eventArgs);
    }

    // reset the topWindowID
    if (m_prevWindowID) {
      winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
          m_reactContext.Properties().Handle(), m_prevWindowID);
      m_prevWindowID = 0;
    }

    m_bridge.Disable();
  }

  // creates a new modal window
  void EnsureModalCreated(const winrt::Microsoft::ReactNative::ComponentView &view) {
    if (m_window) {
      return;
    }

#ifdef USE_EXPERIMENTAL_WINUI3
    if (m_popUp) {
      return;
    }
#endif
    // get the root hwnd
    m_prevWindowID =
        winrt::Microsoft::ReactNative::ReactCoreInjection::GetTopLevelWindowId(view.ReactContext().Properties());

    m_parentHwnd =
        view.as<::Microsoft::ReactNative::Composition::Experimental::IComponentViewInterop>()->GetHwndForParenting();

#ifdef USE_EXPERIMENTAL_WINUI3
    m_bridge = winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(
        view.Parent().as<winrt::Microsoft::ReactNative::Composition::ComponentView>().Compositor(),
        winrt::Microsoft::UI::GetWindowIdFromWindow(m_parentHwnd));
    m_reactNativeIsland = winrt::Microsoft::ReactNative::ReactNativeIsland::CreatePortal(
        view.as<winrt::Microsoft::ReactNative::Composition::PortalComponentView>());
    auto contentIsland = m_reactNativeIsland.Island();

    m_popUp = m_bridge.TryCreatePopupSiteBridge();
    m_popUp.Connect(contentIsland);

    auto navHost = winrt::Microsoft::UI::Input::InputFocusNavigationHost::GetForSiteBridge(
        m_popUp.as<winrt::Microsoft::UI::Content::IContentSiteBridge>());
    m_departFocusToken = navHost.DepartFocusRequested(
        [wkView = winrt::make_weak(view)](
            const auto &sender, const winrt::Microsoft::UI::Input::FocusNavigationRequestEventArgs &args) {
          if (auto strongView = wkView.get()) {
            TrySetFocus(strongView.Parent());
          }
        });

    m_bridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);
#else
    auto presenter = winrt::Microsoft::UI::Windowing::OverlappedPresenter::CreateForDialog();
    presenter.SetBorderAndTitleBar(true, false);
    presenter.IsModal(true);

    m_window = winrt::Microsoft::UI::Windowing::AppWindow::Create(
        presenter, winrt::Microsoft::UI::GetWindowIdFromWindow(m_parentHwnd));

    // set the top-level windows as the new hwnd
    winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
        view.ReactContext().Properties(),
        reinterpret_cast<uint64_t>(winrt::Microsoft::UI::GetWindowFromWindowId(m_window.Id())));

    // create a react native island - code taken from CompositionHwndHost
    m_bridge = winrt::Microsoft::UI::Content::DesktopChildSiteBridge::Create(
        view.Parent().as<winrt::Microsoft::ReactNative::Composition::ComponentView>().Compositor(), m_window.Id());
    m_reactNativeIsland = winrt::Microsoft::ReactNative::ReactNativeIsland::CreatePortal(
        view.as<winrt::Microsoft::ReactNative::Composition::PortalComponentView>());
    auto contentIsland = m_reactNativeIsland.Island();

    auto navHost = winrt::Microsoft::UI::Input::InputFocusNavigationHost::GetForSiteBridge(m_bridge);
    m_departFocusToken = navHost.DepartFocusRequested(
        [wkView = winrt::make_weak(view)](
            const auto &sender, const winrt::Microsoft::UI::Input::FocusNavigationRequestEventArgs &args) {
          if (auto strongView = wkView.get()) {
            TrySetFocus(strongView.Parent());
          }
        });
    m_bridge.Connect(contentIsland);

#endif

    m_bridge.ResizePolicy(winrt::Microsoft::UI::Content::ContentSizePolicy::ResizeContentToParentWindow);
    AdjustWindowSize(view.LayoutMetrics());
    m_bridge.Show();
  }

  static void TrySetFocus(const winrt::Microsoft::ReactNative::ComponentView &view) {
    auto focusController = winrt::Microsoft::UI::Input::InputFocusController::GetForIsland(
        view.as<winrt::Microsoft::ReactNative::Composition::ComponentView>().Root().ReactNativeIsland().Island());
    focusController.TrySetFocus();
  }

  ReactContext m_reactContext{nullptr};
  HWND m_parentHwnd{nullptr};
  winrt::Microsoft::UI::Windowing::AppWindow m_window{nullptr};
  uint64_t m_prevWindowID;
  bool m_showTitleBar{false};
  bool m_showQueued{false};
  bool m_mounted{false};
  winrt::Microsoft::UI::Input::InputFocusNavigationHost::DepartFocusRequested_revoker m_departFocusRevoker;
  winrt::event_token m_departFocusToken;
  winrt::Microsoft::UI::Content::DesktopChildSiteBridge m_bridge{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeIsland m_reactNativeIsland{nullptr};
#ifdef USE_EXPERIMENTAL_WINUI3
  winrt::Microsoft::UI::Content::PopupWindowSiteBridge m_popUp{nullptr};
#endif
};

void RegisterWindowsModalHostNativeComponent(
    winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept {
  ::Microsoft::ReactNativeSpecs::RegisterModalHostViewNativeComponent<ModalHostView>(
      packageBuilder,
      [](const winrt::Microsoft::ReactNative::Composition::IReactCompositionViewComponentBuilder &builder) {
        builder.SetPortalComponentViewInitializer(
            [](const winrt::Microsoft::ReactNative::Composition::PortalComponentView &portalComponentView) noexcept {
              auto userData = winrt::make_self<ModalHostView>();
              userData->InitializePortalViewComponent(portalComponentView);
              portalComponentView.UserData(*userData);
            });
      });
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
