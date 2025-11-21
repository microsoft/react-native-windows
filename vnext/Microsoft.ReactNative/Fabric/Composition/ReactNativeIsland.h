// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "ReactNativeIsland.g.h"
#include <FocusNavigationRequest.g.h>
#include <FocusNavigationResult.g.h>

#include <ReactContext.h>
#include <ReactHost/DebuggerNotifications.h>
#include <react/renderer/core/LayoutConstraints.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.h>
#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include "CompositionEventHandler.h"
#include "DebuggerUIIsland.h"
#include "PortalComponentView.h"
#include "ReactHost/React.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct FocusNavigationRequest : FocusNavigationRequestT<FocusNavigationRequest> {
  FocusNavigationRequest(winrt::Microsoft::ReactNative::FocusNavigationReason reason) : m_reason(reason) {}

  winrt::Microsoft::ReactNative::FocusNavigationReason Reason() noexcept {
    return m_reason;
  }

  void Reason(winrt::Microsoft::ReactNative::FocusNavigationReason reason) noexcept {
    m_reason = reason;
  }

 private:
  winrt::Microsoft::ReactNative::FocusNavigationReason m_reason;
};

struct FocusNavigationResult : FocusNavigationResultT<FocusNavigationResult> {
  FocusNavigationResult(bool wasFocusMoved) : m_wasFocusMoved(wasFocusMoved) {}

  bool WasFocusMoved() noexcept {
    return m_wasFocusMoved;
  }

 private:
  const bool m_wasFocusMoved;
};

struct ReactNativeIsland
    : ReactNativeIslandT<ReactNativeIsland, Composition::Experimental::IInternalCompositionRootView> {
  ReactNativeIsland() noexcept;
  ~ReactNativeIsland() noexcept;

  ReactNativeIsland(const winrt::Microsoft::UI::Composition::Compositor &compositor) noexcept;
  static ReactPropertyId<winrt::Microsoft::ReactNative::ReactNonAbiValue<
      winrt::weak_ref<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>>>
  LastFocusedReactNativeIslandProperty() noexcept;
  ReactNativeIsland(const winrt::Microsoft::ReactNative::Composition::PortalComponentView &portal) noexcept;

  static winrt::Microsoft::ReactNative::ReactNativeIsland CreatePortal(
      const winrt::Microsoft::ReactNative::Composition::PortalComponentView &portal) noexcept;
  winrt::Microsoft::UI::Content::ContentIsland Island();

  // property ReactViewHost
  ReactNative::IReactViewHost ReactViewHost() noexcept;
  void ReactViewHost(ReactNative::IReactViewHost const &value);

  winrt::Microsoft::UI::Composition::Visual RootVisual() noexcept;

  // property RootVisual
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual InternalRootVisual() noexcept;
  void InternalRootVisual(winrt::Microsoft::ReactNative::Composition::Experimental::IVisual const &value) noexcept;

  void OnMounted() noexcept;
  void OnUnmounted() noexcept;

  // property Size
  winrt::Windows::Foundation::Size Size() noexcept;
  void Size(winrt::Windows::Foundation::Size value) noexcept;

  // ScaleFactor (DPI)
  float ScaleFactor() noexcept;
  void ScaleFactor(float value) noexcept;

  void SetProperties(winrt::Microsoft::ReactNative::JSValueArgWriter props) noexcept;

  float FontSizeMultiplier() const noexcept;

  winrt::event_token SizeChanged(
      winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::RootViewSizeChangedEventArgs> const
          &handler) noexcept;
  void SizeChanged(winrt::event_token const &token) noexcept;
  void NotifySizeChanged() noexcept;

  void AddRenderedVisual(const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual) noexcept;
  void RemoveRenderedVisual(const winrt::Microsoft::ReactNative::Composition::Experimental::IVisual &visual) noexcept;
  bool TrySetFocus() noexcept;
  HWND GetHwndForParenting() noexcept;

  winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader Resources() noexcept;
  void Resources(const winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader &resources) noexcept;

  winrt::Microsoft::ReactNative::Composition::Theme Theme() noexcept;
  void Theme(const winrt::Microsoft::ReactNative::Composition::Theme &value) noexcept;

  winrt::Windows::Foundation::Size Measure(
      const winrt::Microsoft::ReactNative::LayoutConstraints &layoutConstraints,
      const winrt::Windows::Foundation::Point &viewportOffset) const;
  void Arrange(
      const winrt::Microsoft::ReactNative::LayoutConstraints &layoutConstraints,
      const winrt::Windows::Foundation::Point &viewportOffset);

  winrt::Microsoft::ReactNative::FocusNavigationResult NavigateFocus(
      const winrt::Microsoft::ReactNative::FocusNavigationRequest &request) noexcept;

  winrt::com_ptr<winrt::Microsoft::ReactNative::Composition::implementation::RootComponentView>
  GetComponentView() noexcept;

  int64_t RootTag() const noexcept;

  IInspectable GetUiaProvider() noexcept;

  // When driving the rootview without an island
  void SetWindow(uint64_t hwnd) noexcept;
  int64_t SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept;

  winrt::Windows::Foundation::Point ConvertScreenToLocal(winrt::Windows::Foundation::Point pt) noexcept;
  winrt::Windows::Foundation::Point ConvertLocalToScreen(winrt::Windows::Foundation::Point pt) noexcept;

  bool CapturePointer(
      const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer,
      facebook::react::Tag tag) noexcept;
  void ReleasePointerCapture(
      const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer,
      facebook::react::Tag tag) noexcept;

  void AddFragmentCompositionEventHandler(
      winrt::Microsoft::ReactNative::IReactContext context,
      winrt::Microsoft::ReactNative::ComponentView componentView) noexcept;

 public: // IReactViewInstance UI-thread implementation
  void InitRootView(
      winrt::Microsoft::ReactNative::IReactContext &&context,
      winrt::Microsoft::ReactNative::ReactViewOptions &&viewOptions) noexcept;
  void UpdateRootView() noexcept;
  void UninitRootView() noexcept;

 private:
  winrt::Microsoft::UI::Composition::Compositor m_compositor{nullptr};
  winrt::Microsoft::UI::Content::ContentIsland m_island{nullptr};
  winrt::event_token m_islandFrameworkClosedToken;
  winrt::event_token m_islandAutomationProviderRequestedToken;
  winrt::event_token m_islandStateChangedToken;
  winrt::event_token m_islandConnectedToken;
  winrt::event_token m_islandDisconnectedToken;

  HWND m_hwnd{0};
  bool m_isFragment{false};
  bool m_isInitialized{false};
  bool m_isJSViewAttached{false};
  bool m_hasRenderedVisual{false};
  bool m_showingLoadingUI{false};
  bool m_mounted{true};
  winrt::weak_ref<winrt::Microsoft::ReactNative::Composition::PortalComponentView> m_portal{nullptr};
  IReactDispatcher m_uiDispatcher{nullptr};
  winrt::IInspectable m_uiaProvider{nullptr};

  // If SetProps is called before the surface is loaded, store it locally to use on start
  folly::dynamic m_props;

  // This is the surfaceId that this island belongs to.
  // In the case of portal content root, this will be the surfaceId that contains the portal.
  int64_t m_rootTag{-1};
  float m_scaleFactor{1.0};
  float m_textScaleMultiplier{1.0};
  winrt::Windows::UI::ViewManagement::UISettings::TextScaleFactorChanged_revoker m_textScaleChangedRevoker;
  winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{nullptr};
  winrt::Windows::Foundation::Size m_size{0, 0};
  winrt::Microsoft::ReactNative::ReactContext m_context;
  winrt::Microsoft::ReactNative::IReactViewHost m_reactViewHost;
  winrt::Microsoft::ReactNative::ReactViewOptions m_reactViewOptions;
  std::shared_ptr<::Microsoft::ReactNative::CompositionEventHandler> m_CompositionEventHandler;
  winrt::Microsoft::ReactNative::Composition::Experimental::IVisual m_rootVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual m_loadingVisual{nullptr};
  winrt::Microsoft::UI::Content::ChildSiteLink m_debuggerChildSiteLink{nullptr};
  std::shared_ptr<DebuggerUIIsland> m_debuggerUIIsland;
  winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual m_debuggerVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::Experimental::IActivityVisual m_loadingActivityVisual{nullptr};
  winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader m_resources{nullptr};
  winrt::Microsoft::ReactNative::Composition::Theme m_theme{nullptr};
  winrt::Microsoft::ReactNative::Composition::Theme::ThemeChanged_revoker m_themeChangedRevoker;
  winrt::Microsoft::ReactNative::LayoutConstraints m_layoutConstraints;
  winrt::Windows::Foundation::Point m_viewportOffset{0, 0};
  winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Microsoft::ReactNative::RootViewSizeChangedEventArgs>>
      m_sizeChangedEvent;

  void UpdateRootViewInternal() noexcept;
  void ClearLoadingUI() noexcept;
  void EnsureLoadingUI() noexcept;
  void ShowInstanceLoaded() noexcept;
  void ShowInstanceError() noexcept;
  void ShowInstanceLoading() noexcept;
  void ShowDebuggerUI(std::string message, const std::function<void()> &onResume) noexcept;
  void HideDebuggerUI() noexcept;
  void UpdateRootVisualSize() noexcept;
  void UpdateLoadingVisualSize() noexcept;
  void UpdateDebuggerVisualSize() noexcept;
  Composition::Experimental::IDrawingSurfaceBrush CreateLoadingVisualBrush() noexcept;
  void ApplyConstraints(
      const winrt::Microsoft::ReactNative::LayoutConstraints &layoutConstraintsIn,
      facebook::react::LayoutConstraints &layoutConstraintsOut) const noexcept;
  facebook::react::Size MeasureLoading(
      const winrt::Microsoft::ReactNative::LayoutConstraints &layoutConstraints) const noexcept;
  void InitTextScaleMultiplier() noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactNativeIsland : ReactNativeIslandT<ReactNativeIsland, implementation::ReactNativeIsland> {};
struct FocusNavigationRequest
    : FocusNavigationRequestT<FocusNavigationRequest, implementation::FocusNavigationRequest> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
