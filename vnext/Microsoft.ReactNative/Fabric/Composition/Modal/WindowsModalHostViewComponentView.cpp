// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsModalHostViewComponentView.h"

#include "../../../codegen/react/components/rnwcore/ModalHostView.g.h"
#include <ComponentView.Experimental.interop.h>
#include <debugapi.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Windowing.h>
#include <winrt/Microsoft.UI.interop.h>
#include <winrt/Windows.Foundation.h>
#include <winuser.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct ModalHostState
    : winrt::implements<ModalHostState, winrt::Microsoft::ReactNative::Composition::IPortalStateData> {
  ModalHostState(winrt::Microsoft::ReactNative::LayoutConstraints layoutConstraints, float scaleFactor)
      : m_layoutConstraints(layoutConstraints), m_pointScaleFactor(scaleFactor) {}

  winrt::Microsoft::ReactNative::LayoutConstraints LayoutConstraints() const noexcept {
    return m_layoutConstraints;
  }

  float PointScaleFactor() const noexcept {
    return m_pointScaleFactor;
  }

 private:
  float m_pointScaleFactor{1.0f};
  winrt::Microsoft::ReactNative::LayoutConstraints m_layoutConstraints;
};

struct ModalHostView : public winrt::implements<ModalHostView, winrt::Windows::Foundation::IInspectable>,
                       ::Microsoft::ReactNativeSpecs::BaseModalHostView<ModalHostView> {
  ~ModalHostView() {
    if (m_reactNativeIsland) {
      m_reactNativeIsland.Island().Close();
    }

    // Add AppWindow closing token cleanup
    if (m_appWindow && m_appWindowClosingToken) {
      m_appWindow.Closing(m_appWindowClosingToken);
      m_appWindowClosingToken.value = 0;
    }

    if (m_popUp) {
      if (m_departFocusToken && !m_popUp.IsClosed()) {
        // WASDK BUG: InputFocusNavigationHost::GetForSiteBridge fails on a DesktopPopupSiteBridge
        // https://github.com/microsoft/react-native-windows/issues/14604
        /*
        auto navHost =
        winrt::Microsoft::UI::Input::InputFocusNavigationHost::GetForSiteBridge(m_popUp.as<winrt::Microsoft::UI::Content::IContentSiteBridge>());
        navHost.DepartFocusRequested(m_departFocusToken);
        */
      }
      m_popUp.Close();
      m_popUp = nullptr;
    }
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
    // Store the props locally
    m_localProps = newProps;

    const auto &oldViewProps = *oldProps;
    const auto &newViewProps = *newProps;

    if (!oldProps || newViewProps.visible != oldViewProps.visible) {
      if (newProps->visible.value_or(true)) {
        m_visible = true;
        // We do not immediately show the window, since we want to resize/position
        // the window based on the layout metrics before we show it
        QueueShow(view);
      } else {
        m_visible = false;
        CloseWindow();
      }
    }

    // Update Title if changed and AppWindow exists
    if (m_appWindow && (!oldProps || newViewProps.title != oldViewProps.title)) {
      winrt::hstring titleValue = winrt::to_hstring(newViewProps.title.value_or("RNW Modal"));
      OutputDebugStringW(L"[ModalHostView] Updating title: ");
      OutputDebugStringW(titleValue.c_str());
      OutputDebugStringW(L"\n");
      m_appWindow.Title(titleValue);
    }

    ::Microsoft::ReactNativeSpecs::BaseModalHostView<ModalHostView>::UpdateProps(view, newProps, oldProps);
  }

  void QueueShow(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
    if (m_showQueued)
      return;
    m_showQueued = true;

    m_reactContext.UIDispatcher().Post([wkThis = get_weak(), wkView = winrt::weak_ref(view)]() {
      if (auto strongThis = wkThis.get()) {
        strongThis->m_showQueued = false;

        if (!strongThis->m_visible) {
          return;
        }
        if (auto v = wkView.get()) {
          strongThis->ShowOnUIThread(v);
        }
      }
    });
  }

  void UpdateState(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::IComponentState &newState) noexcept override {
    m_state = newState;
  }

  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::MountChildComponentViewArgs &args) noexcept override {
    AdjustWindowSize(args.Child().LayoutMetrics());
    assert(!m_childLayoutMetricsToken);
    m_childLayoutMetricsToken = args.Child().LayoutMetricsChanged(
        [wkThis = get_weak()](
            auto &sender, const winrt::Microsoft::ReactNative::LayoutMetricsChangedArgs &layoutMetricsChangedArgs) {
          if (auto strongThis = wkThis.get()) {
            strongThis->AdjustWindowSize(layoutMetricsChangedArgs.NewLayoutMetrics());
          }
        });
  }

  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView & /*view*/,
      const winrt::Microsoft::ReactNative::UnmountChildComponentViewArgs &args) noexcept override {
    assert(m_childLayoutMetricsToken);
    args.Child().LayoutMetricsChanged(m_childLayoutMetricsToken);
    m_childLayoutMetricsToken.value = 0;
  }

 private:
  void OnMounted(const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
    m_mounted = true;
    if (m_visible) {
      QueueShow(view);
    }
  }

  void OnUnmounted(const winrt::Microsoft::ReactNative::ComponentView & /*view*/) noexcept {
    m_mounted = false;
  }

  void AdjustWindowSize(const winrt::Microsoft::ReactNative::LayoutMetrics &layoutMetrics) noexcept {
    // Revert to the simpler version before non-client calculation
    if (!m_popUp) {
      return;
    }

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

    winrt::Windows::Graphics::RectInt32 rect2{
        (int)xCor,
        (int)yCor,
        static_cast<int32_t>(layoutMetrics.Frame.Width * (layoutMetrics.PointScaleFactor)),
        static_cast<int32_t>(layoutMetrics.Frame.Height * (layoutMetrics.PointScaleFactor))};

    // Log the dimensions for debugging
    OutputDebugStringW(L"[ModalHostView] AdjustWindowSize Resizing to: ");
    OutputDebugStringW((winrt::to_hstring(rect2.Width) + L"x" + winrt::to_hstring(rect2.Height) + L"\n").c_str());

    m_popUp.MoveAndResize(rect2);
  };

  void ShowOnUIThread(const winrt::Microsoft::ReactNative::ComponentView &view) {
    if (!m_mounted)
      return;

    EnsureModalCreated(view);

    if (m_popUp) {
      m_popUp.Show();

      // WASDK BUG: InputFocusNavigationHost::GetForSiteBridge fails on a DesktopPopupSiteBridge
      // https://github.com/microsoft/react-native-windows/issues/14604
      /*
      auto navHost = winrt::Microsoft::UI::Input::InputFocusNavigationHost::GetForSiteBridge(
          m_popUp.as<winrt::Microsoft::UI::Content::IContentSiteBridge>());
      auto result = navHost.NavigateFocus(winrt::Microsoft::UI::Input::FocusNavigationRequest::Create(
          winrt::Microsoft::UI::Input::FocusNavigationReason::First));
      */

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
    if (m_popUp) {
      m_popUp.Hide();
    }

    // Unregister closing event handler
    if (m_appWindow && m_appWindowClosingToken) {
      m_appWindow.Closing(m_appWindowClosingToken);
      m_appWindowClosingToken.value = 0;
    }

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
  }

  // creates a new modal window
  void EnsureModalCreated(const winrt::Microsoft::ReactNative::ComponentView &view) {
    if (m_popUp) {
      return;
    }
    // get the root hwnd
    m_prevWindowID =
        winrt::Microsoft::ReactNative::ReactCoreInjection::GetTopLevelWindowId(view.ReactContext().Properties());

    m_parentHwnd =
        view.as<::Microsoft::ReactNative::Composition::Experimental::IComponentViewInterop>()->GetHwndForParenting();

    auto portal = view.as<winrt::Microsoft::ReactNative::Composition::PortalComponentView>();
    m_reactNativeIsland = winrt::Microsoft::ReactNative::ReactNativeIsland::CreatePortal(portal);
    auto contentIsland = m_reactNativeIsland.Island();

    m_popUp = winrt::Microsoft::UI::Content::DesktopPopupSiteBridge::Create(
        portal.Parent()
            .as<winrt::Microsoft::ReactNative::Composition::ComponentView>()
            .Root()
            .ReactNativeIsland()
            .Island());
    m_popUp.Connect(contentIsland);

    // Get AppWindow and configure presenter
    m_appWindow = winrt::Microsoft::UI::Windowing::AppWindow::GetFromWindowId(m_popUp.WindowId());
    if (m_appWindow) {
      // Log the initial presenter type
      OutputDebugStringW(L"[ModalHostView] AppWindow Presenter Kind: ");
      OutputDebugStringW(winrt::to_hstring((int32_t)m_appWindow.Presenter().Kind()).c_str());
      OutputDebugStringW(L"\n");
      auto overlappedPresenter = winrt::Microsoft::UI::Windowing::OverlappedPresenter::Create();

      // Configure presenter for modal behavior
      overlappedPresenter.IsModal(true);
      overlappedPresenter.IsResizable(true);
      overlappedPresenter.SetBorderAndTitleBar(true, true);

      // Apply the presenter to the window
      m_appWindow.SetPresenter(overlappedPresenter);
      OutputDebugStringW(L"[ModalHostView] Created and set new OverlappedPresenter.\n");

      // Set initial title using the stored local props
      if (m_localProps) {
        winrt::hstring titleValue = winrt::to_hstring(m_localProps->title.value_or("RNW Modal"));
        OutputDebugStringW(L"[ModalHostView] Setting initial title: ");
        OutputDebugStringW(titleValue.c_str());
        OutputDebugStringW(L"\n");

        m_appWindow.Title(titleValue);

        // Double check if the title was set
        OutputDebugStringW(L"[ModalHostView] Current window title: ");
        OutputDebugStringW(m_appWindow.Title().c_str());
        OutputDebugStringW(L"\n");
      } else {
        OutputDebugStringW(L"[ModalHostView] Setting default title (no props yet).\n");
        m_appWindow.Title(L"RNW Modal"); // Set a default title
      }

      // Handle close request ('X' button)
      m_appWindowClosingToken =
          m_appWindow.Closing([wkThis = get_weak()](
                                  const winrt::Microsoft::UI::Windowing::AppWindow & /*sender*/,
                                  const winrt::Microsoft::UI::Windowing::AppWindowClosingEventArgs &args) {
            OutputDebugStringW(L"[ModalHostView] AppWindow Closing event received.\n");
            args.Cancel(true); // Prevent default close
            if (auto strongThis = wkThis.get()) {
              // Dispatch onRequestClose event
              if (auto eventEmitter = strongThis->EventEmitter()) {
                OutputDebugStringW(L"[ModalHostView] Dispatching onRequestClose event.\n");
                ::Microsoft::ReactNativeSpecs::ModalHostViewEventEmitter::OnRequestClose eventArgs;
                eventEmitter->onRequestClose(eventArgs);
              }
            }
          });
    } else {
      OutputDebugStringW(L"[ModalHostView] Failed to get AppWindow from WindowId.\n");
    }

    // set the top-level windows as the new hwnd
    winrt::Microsoft::ReactNative::ReactCoreInjection::SetTopLevelWindowId(
        view.ReactContext().Properties(),
        reinterpret_cast<uint64_t>(winrt::Microsoft::UI::GetWindowFromWindowId(m_popUp.WindowId())));

    // WASDK BUG: InputFocusNavigationHost::GetForSiteBridge fails on a DesktopPopupSiteBridge
    // https://github.com/microsoft/react-native-windows/issues/14604
    /*
    auto navHost = winrt::Microsoft::UI::Input::InputFocusNavigationHost::GetForSiteBridge(
        m_popUp.as<winrt::Microsoft::UI::Content::IContentSiteBridge>());
    m_departFocusToken = navHost.DepartFocusRequested(
        [wkView = winrt::make_weak(view)](
            const auto &sender, const winrt::Microsoft::UI::Input::FocusNavigationRequestEventArgs &args) {
          if (auto strongView = wkView.get()) {
            TrySetFocus(strongView.Parent());
          }
        });
        */

    m_islandStateChangedToken =
        contentIsland.StateChanged([weakThis = get_weak()](
                                       winrt::Microsoft::UI::Content::ContentIsland const &island,
                                       winrt::Microsoft::UI::Content::ContentIslandStateChangedEventArgs const &args) {
          if (auto pThis = weakThis.get()) {
            if (args.DidRasterizationScaleChange() || args.DidLayoutDirectionChange()) {
              pThis->UpdateConstraints();
            }
          }
        });

    UpdateConstraints();

    if (portal.ContentRoot().Children().Size()) {
      AdjustWindowSize(portal.ContentRoot().Children().GetAt(0).LayoutMetrics());
    }
  }

  void UpdateConstraints() noexcept {
    auto displayArea = winrt::Microsoft::UI::Windowing::DisplayArea::GetFromDisplayId(
        m_popUp.SiteView().EnvironmentView().DisplayId());
    auto workArea = displayArea.WorkArea();

    float scale = m_reactNativeIsland.Island().RasterizationScale();

    winrt::Microsoft::ReactNative::LayoutConstraints constraints;
    constraints.MinimumSize = {0, 0};
    // Constrain the size of the modal to 90% of the screen size
    constraints.MaximumSize = {
        static_cast<float>((workArea.Width / scale) * 0.9), static_cast<float>((workArea.Height / scale) * 0.9)};
    constraints.LayoutDirection = winrt::Microsoft::ReactNative::LayoutDirection::Undefined;

    auto layoutDirection = m_reactNativeIsland.Island().LayoutDirection();
    if (layoutDirection == winrt::Microsoft::UI::Content::ContentLayoutDirection::LeftToRight)
      constraints.LayoutDirection = winrt::Microsoft::ReactNative::LayoutDirection::LeftToRight;
    else if (layoutDirection == winrt::Microsoft::UI::Content::ContentLayoutDirection::RightToLeft)
      constraints.LayoutDirection = winrt::Microsoft::ReactNative::LayoutDirection::RightToLeft;

    // By setting a custom contraint here the behavior of the modal slightly changes.
    // When no constraint is set (maxSize is std::numeric_limits<Float>::infinity()), yoga will layout the content to a
    // desired size If we provide a specific max size, then contents with a flex:1 will expand to fill that size. We
    // might want to provide a windows specific property to control this behavior.
    m_state.UpdateState(winrt::make<ModalHostState>(constraints, m_reactNativeIsland.Island().RasterizationScale()));
  }

  static void TrySetFocus(const winrt::Microsoft::ReactNative::ComponentView &view) {
    auto focusController = winrt::Microsoft::UI::Input::InputFocusController::GetForIsland(
        view.as<winrt::Microsoft::ReactNative::Composition::ComponentView>().Root().ReactNativeIsland().Island());
    focusController.TrySetFocus();
  }

  ReactContext m_reactContext{nullptr};
  HWND m_parentHwnd{nullptr};
  uint64_t m_prevWindowID;
  bool m_showQueued{false};
  bool m_visible{false};
  bool m_mounted{false};
  winrt::event_token m_islandStateChangedToken;
  winrt::Microsoft::UI::Input::InputFocusNavigationHost::DepartFocusRequested_revoker m_departFocusRevoker;
  winrt::event_token m_departFocusToken;
  winrt::event_token m_childLayoutMetricsToken;
  winrt::Microsoft::ReactNative::IComponentState m_state{nullptr};
  winrt::Microsoft::ReactNative::ReactNativeIsland m_reactNativeIsland{nullptr};
  winrt::Microsoft::UI::Content::DesktopPopupSiteBridge m_popUp{nullptr};
  winrt::Microsoft::UI::Windowing::AppWindow m_appWindow{nullptr};
  winrt::event_token m_appWindowClosingToken;
  winrt::com_ptr<::Microsoft::ReactNativeSpecs::ModalHostViewProps> m_localProps{nullptr};
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
