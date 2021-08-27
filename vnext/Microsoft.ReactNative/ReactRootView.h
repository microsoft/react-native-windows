// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "ReactRootView.g.h"

#include "ReactHost/React.h"
#include "SIPEventHandler.h"
#include "TouchEventHandler.h"
#include "Views/IXamlRootView.h"
#include "Views/KeyboardEventHandler.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct ReactRootView : ReactRootViewT<ReactRootView>, ::Microsoft::ReactNative::IXamlRootView {
  ReactRootView() noexcept;

  // property ReactNativeHost
  ReactNative::ReactNativeHost ReactNativeHost() noexcept;
  void ReactNativeHost(ReactNative::ReactNativeHost const &value) noexcept;

  // property ComponentName
  hstring ComponentName() noexcept;
  void ComponentName(hstring const &value) noexcept;

  // property InitialProps
  ReactNative::JSValueArgWriter InitialProps() noexcept;
  void InitialProps(ReactNative::JSValueArgWriter const &value) noexcept;

  // property IsPerspectiveEnabled
  bool IsPerspectiveEnabled() const noexcept {
    return m_isPerspectiveEnabled;
  }
  void IsPerspectiveEnabled(bool value) noexcept {
    m_isPerspectiveEnabled = value;
    UpdatePerspective();
  }

  // property ExperimentalUseFabric
  bool ExperimentalUseFabric() const noexcept;
  void ExperimentalUseFabric(bool value) noexcept;

  void ReloadView() noexcept;

 public: // IXamlRootView
  ::Microsoft::ReactNative::XamlView GetXamlView() const noexcept override;

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

  Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size const &availableSize) const;
  Windows::Foundation::Size ArrangeOverride(Windows::Foundation::Size finalSize) const;

  void blur(::Microsoft::ReactNative::XamlView const &xamlView) noexcept;

 private:
  ReactNative::ReactNativeHost m_reactNativeHost{nullptr};
  hstring m_componentName;
  ReactNative::JSValueArgWriter m_initialPropsWriter;
  bool m_isPerspectiveEnabled{true};
  bool m_isInitialized{false};
  bool m_isJSViewAttached{false};
  bool m_useFabric{false};
  Mso::DispatchQueue m_uiQueue;
  int64_t m_rootTag{-1};
  std::unique_ptr<Mso::React::ReactOptions> m_reactOptions;
  Mso::WeakPtr<Mso::React::IReactInstance> m_weakReactInstance;
  Mso::CntPtr<Mso::React::IReactContext> m_context;
  Mso::CntPtr<Mso::React::IReactViewHost> m_reactViewHost;
  std::unique_ptr<Mso::React::ReactViewOptions> m_reactViewOptions;
  std::shared_ptr<::Microsoft::ReactNative::TouchEventHandler> m_touchEventHandler;
  std::shared_ptr<::Microsoft::ReactNative::SIPEventHandler> m_SIPEventHandler;
  std::shared_ptr<::Microsoft::ReactNative::PreviewKeyboardEventHandlerOnRoot> m_previewKeyboardEventHandlerOnRoot;
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

  void UpdatePerspective();
  void UpdateRootViewInternal() noexcept;
  void ClearLoadingUI() noexcept;
  void EnsureLoadingUI() noexcept;
  void ShowInstanceLoaded() noexcept;
  void ShowInstanceError() noexcept;
  void ShowInstanceWaiting() noexcept;
  void ShowInstanceLoading() noexcept;
  void EnsureFocusSafeHarbor() noexcept;
  void AttachBackHandlers() noexcept;
  void RemoveBackHandlers() noexcept;
  bool OnBackRequested() noexcept;
  Mso::React::IReactViewHost *ReactViewHost() noexcept;
  void ReactViewHost(Mso::React::IReactViewHost *viewHost) noexcept;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct ReactRootView : ReactRootViewT<ReactRootView, implementation::ReactRootView> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
