// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "CompRootView.g.h"

#include "ReactHost/React.h"
#include "SIPEventHandler.h"
#include "TouchEventHandler.h"
#include "Views/ICompRootView.h"
#include "Views/KeyboardEventHandler.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct CompRootView : CompRootViewT<CompRootView>, ::Microsoft::ReactNative::ICompRootView {
  CompRootView() noexcept;

  // property ReactNativeHost
  ReactNative::ReactNativeHost ReactNativeHost() noexcept;
  void ReactNativeHost(ReactNative::ReactNativeHost const &value) noexcept;

  // property ComponentName
  hstring ComponentName() noexcept;
  void ComponentName(hstring const &value) noexcept;

  // property Compositor
  winrt::Windows::UI::Composition::Compositor Compositor() noexcept;
  void Compositor(winrt::Windows::UI::Composition::Compositor const & value) noexcept;

  // property RootVisual
  winrt::Windows::UI::Composition::Visual RootVisual() noexcept;
  void RootVisual(winrt::Windows::UI::Composition::Visual const & value) noexcept;

  // property InitialProps
  ReactNative::JSValueArgWriter InitialProps() noexcept;
  void InitialProps(ReactNative::JSValueArgWriter const &value) noexcept;

  // property Size
  winrt::Windows::Foundation::Size Size() noexcept;
  void Size(winrt::Windows::Foundation::Size value) noexcept;

  // ScaleFactor (DPI)
  double ScaleFactor() noexcept;
  void ScaleFactor(double value) noexcept;

  void ReloadView() noexcept;

  Windows::Foundation::Size Measure(Windows::Foundation::Size const &availableSize) const;
  Windows::Foundation::Size Arrange(Windows::Foundation::Size finalSize) const;

  void OnMouseDown(Windows::Foundation::Point point) noexcept;
  void OnMouseUp() noexcept;

  // Used by RootViewManager
  /*
  void AddView(uint32_t index, xaml::UIElement child);
  void RemoveAllChildren();
  void RemoveChildAt(uint32_t index);

 public: // IXamlRootView
  ::Microsoft::ReactNative::XamlView GetXamlView() const noexcept override;
  */
 public: // ICompRootView
  winrt::Windows::UI::Composition::Visual GetVisual() const noexcept override;
  //winrt::Windows::UI::Composition::Compositor Compositor() const noexcept override;

 public: // IReactRootView
  std::string JSComponentName() const noexcept override;
  int64_t GetActualHeight() const noexcept override;
  int64_t GetActualWidth() const noexcept override;
  int64_t GetTag() const noexcept override;
  void SetTag(int64_t tag) noexcept override;

 public: // IReactViewInstance UI-thread implementation
  void InitRootView(
      Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
      Mso::React::ReactViewOptions &&viewOptions) noexcept;
  void UpdateRootView() noexcept;
  void UninitRootView() noexcept;

/*
  Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size const &availableSize) const;
  Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size finalSize) const;

  void blur(::Microsoft::ReactNative::XamlView const &xamlView) noexcept;
*/
 private:
  ReactNative::ReactNativeHost m_reactNativeHost{nullptr};
  winrt::Windows::UI::Composition::Compositor m_compositor{nullptr};
  hstring m_componentName;
  ReactNative::JSValueArgWriter m_initialPropsWriter;
  bool m_isPerspectiveEnabled{true};
  bool m_isInitialized{false};
  bool m_isJSViewAttached{false};
  Mso::DispatchQueue m_uiQueue;
  int64_t m_rootTag{-1};
  double m_scaleFactor{1.0};
  winrt::Windows::Foundation::Size m_size;
  std::unique_ptr<Mso::React::ReactOptions> m_reactOptions;
  Mso::WeakPtr<Mso::React::IReactInstance> m_weakReactInstance;
  Mso::CntPtr<Mso::React::IReactContext> m_context;
  Mso::CntPtr<Mso::React::IReactViewHost> m_reactViewHost;
  std::unique_ptr<Mso::React::ReactViewOptions> m_reactViewOptions;
  std::shared_ptr<::Microsoft::ReactNative::TouchEventHandler> m_touchEventHandler;
  /*
  xaml::Controls::ContentControl m_focusSafeHarbor{nullptr};
  xaml::Controls::ContentControl::LosingFocus_revoker m_focusSafeHarborLosingFocusRevoker{};
  xaml::FrameworkElement::SizeChanged_revoker m_rootSizeChangedRevoker{};
  winrt::Grid m_greenBoxGrid{nullptr};
  winrt::TextBlock m_waitingTextBlock{nullptr};
  winrt::SystemNavigationManager::BackRequested_revoker m_backRequestedRevoker{};

  // Visual tree to support safe harbor
  // this
  //   m_focusSafeHarbor
  //   m_greenBoxGrid
  //     m_waitingTextBlock (loading string)
  //   m_xamlRootView
  //     JS created children
  winrt::Grid m_xamlRootView{nullptr};


  uint32_t RNIndexToXamlIndex(uint32_t index) noexcept;
  void UpdatePerspective();
  */
  winrt::Windows::UI::Composition::Visual m_rootVisual{nullptr};
  void UpdateRootViewInternal() noexcept;
  void ClearLoadingUI() noexcept;
  void EnsureLoadingUI() noexcept;
  void ShowInstanceLoaded() noexcept;
  void ShowInstanceError() noexcept;
  void ShowInstanceWaiting() noexcept;
  void ShowInstanceLoading() noexcept;
  Mso::React::IReactViewHost *ReactViewHost() noexcept;
  void ReactViewHost(Mso::React::IReactViewHost *viewHost) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct CompRootView : CompRootViewT<CompRootView, implementation::CompRootView> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
