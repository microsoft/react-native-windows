// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "CompositionRootView.g.h"

#include <ReactContext.h>
#include <winrt/Microsoft.ReactNative.h>
#include "CompositionEventHandler.h"
#include "ReactHost/React.h"
#include "Views/ICompositionRootView.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct CompositionRootView : CompositionRootViewT<CompositionRootView>, ::Microsoft::ReactNative::ICompositionRootView {
  CompositionRootView() noexcept;

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
  double ScaleFactor() noexcept;
  void ScaleFactor(double value) noexcept;

  Windows::Foundation::Size Measure(Windows::Foundation::Size const &availableSize) const;
  Windows::Foundation::Size Arrange(Windows::Foundation::Size finalSize) const;

  int64_t SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept;
  void OnScrollWheel(Windows::Foundation::Point point, int32_t delta) noexcept;

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
  bool m_isInitialized{false};
  bool m_isJSViewAttached{false};
  IReactDispatcher m_uiDispatcher{nullptr};
  int64_t m_rootTag{-1};
  double m_scaleFactor{1.0};
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
} // namespace winrt::Microsoft::ReactNative::factory_implementation
