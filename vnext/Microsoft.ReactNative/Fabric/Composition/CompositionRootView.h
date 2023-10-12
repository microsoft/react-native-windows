// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "CompositionRootView.g.h"
#include <FocusNavigationRequest.g.h>
#include <FocusNavigationResult.g.h>

#include <ReactContext.h>
#include <winrt/Microsoft.ReactNative.h>
#include "CompositionEventHandler.h"
#include "ReactHost/React.h"
#include "Views/ICompositionRootView.h"

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

struct CompositionRootView : CompositionRootViewT<CompositionRootView>, ::Microsoft::ReactNative::ICompositionRootView {
  CompositionRootView() noexcept;

#ifdef USE_WINUI3
  CompositionRootView(winrt::Microsoft::UI::Composition::Compositor compositor) noexcept;
  winrt::Microsoft::UI::Content::ContentIsland Island() noexcept;
#endif

  // property ReactViewHost
  ReactNative::IReactViewHost ReactViewHost() noexcept;
  void ReactViewHost(ReactNative::IReactViewHost const &value) noexcept;

  // property RootVisual
  winrt::Microsoft::ReactNative::Composition::IVisual RootVisual() noexcept;
  void RootVisual(winrt::Microsoft::ReactNative::Composition::IVisual const &value) noexcept;

  // property Size
  winrt::Windows::Foundation::Size Size() noexcept;
  void Size(winrt::Windows::Foundation::Size value) noexcept;

  // ScaleFactor (DPI)
  float ScaleFactor() noexcept;
  void ScaleFactor(float value) noexcept;

  winrt::Windows::Foundation::Size Measure(winrt::Windows::Foundation::Size const &availableSize) const;
  winrt::Windows::Foundation::Size Arrange(winrt::Windows::Foundation::Size finalSize) const;

  winrt::Microsoft::ReactNative::FocusNavigationResult NavigateFocus(
      const winrt::Microsoft::ReactNative::FocusNavigationRequest &request) noexcept;

  ::Microsoft::ReactNative::RootComponentView *GetComponentView() noexcept;

  IInspectable GetUiaProvider() noexcept;

  int64_t SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept;

 public: // ICompositionRootView
  winrt::Microsoft::ReactNative::Composition::IVisual GetVisual() const noexcept override;

 public: // IReactRootView
  std::string JSComponentName() const noexcept override;
  int64_t GetActualHeight() const noexcept override;
  int64_t GetActualWidth() const noexcept override;
  int64_t GetTag() const noexcept override;
  void SetTag(int64_t tag) noexcept override;

 public: // IReactViewInstance UI-thread implementation
  void InitRootView(
      winrt::Microsoft::ReactNative::IReactContext &&context,
      winrt::Microsoft::ReactNative::ReactViewOptions &&viewOptions) noexcept;
  void UpdateRootView() noexcept;
  void UninitRootView() noexcept;

 private:
#ifdef USE_WINUI3
  winrt::Microsoft::UI::Composition::Compositor m_compositor{nullptr};
  winrt::Microsoft::UI::Content::ContentIsland m_island{nullptr};
#endif

  bool m_isInitialized{false};
  bool m_isJSViewAttached{false};
  IReactDispatcher m_uiDispatcher{nullptr};
  int64_t m_rootTag{-1};
  float m_scaleFactor{1.0};
  winrt::Windows::Foundation::Size m_size;
  winrt::Microsoft::ReactNative::ReactContext m_context;
  winrt::Microsoft::ReactNative::IReactViewHost m_reactViewHost;
  winrt::Microsoft::ReactNative::ReactViewOptions m_reactViewOptions;
  std::shared_ptr<::Microsoft::ReactNative::CompositionEventHandler> m_CompositionEventHandler;
  winrt::Microsoft::ReactNative::Composition::IVisual m_rootVisual{nullptr};
  void UpdateRootViewInternal() noexcept;
  void ClearLoadingUI() noexcept;
  void EnsureLoadingUI() noexcept;
  void ShowInstanceLoaded() noexcept;
  void ShowInstanceError() noexcept;
  void ShowInstanceLoading() noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct CompositionRootView : CompositionRootViewT<CompositionRootView, implementation::CompositionRootView> {};
struct FocusNavigationRequest
    : FocusNavigationRequestT<FocusNavigationRequest, implementation::FocusNavigationRequest> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
