// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <memory>

#include <IReactInstance.h>
#include <object/unknownObject.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include "IXamlRootView.h"
#include "ReactHost/React.h"
#include "SIPEventHandler.h"
#include "TouchEventHandler.h"
#include "Views/KeyboardEventHandler.h"

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt

namespace react::uwp {

// A class that implements IXamlRootView and IXamlReactControl interfaces.
// It keeps a weak reference to the XAML ReactRootView.
// TODO: consider to remove this class in favor of ReactRootView XAML control.
struct ReactRootControl final : std::enable_shared_from_this<ReactRootControl>, IXamlRootView, IXamlReactControl {
  ReactRootControl(XamlView const &rootView) noexcept;
  ~ReactRootControl() noexcept;

 public: // IXamlRootView
  std::shared_ptr<IReactInstance> GetReactInstance() const noexcept override;
  XamlView GetXamlView() const noexcept override;
  void SetJSComponentName(std::string &&mainComponentName) noexcept override;
  void SetInstanceCreator(const ReactInstanceCreator &instanceCreator) noexcept override;
  void SetInitialProps(folly::dynamic &&initialProps) noexcept override;
  void AttachRoot() noexcept override;
  void DetachRoot() noexcept override;
  std::shared_ptr<IXamlReactControl> GetXamlReactControl() const noexcept override;

 public: // IReactRootView
  void ResetView() noexcept override;
  std::string JSComponentName() const noexcept override;
  int64_t GetActualHeight() const noexcept override;
  int64_t GetActualWidth() const noexcept override;
  int64_t GetTag() const noexcept override;
  void SetTag(int64_t tag) noexcept override;

 public: // IXamlReactControl
  void blur(XamlView const &xamlView) noexcept override;

 public:
  //! property ReactViewHost : Mso::React::IReactViewHost
  Mso::React::IReactViewHost *ReactViewHost() noexcept;
  void ReactViewHost(Mso::React::IReactViewHost *viewHost) noexcept;

 public: // IReactViewInstance UI-thread implementation
  void InitRootView(
      Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
      Mso::React::ReactViewOptions &&viewOptions) noexcept;
  void UpdateRootView() noexcept;
  void UninitRootView() noexcept;

 private:
  void PrepareXamlRootView(XamlView const &rootView) noexcept;
  void EnsureFocusSafeHarbor() noexcept;
  void UpdateRootViewInternal() noexcept;

  void ShowInstanceLoading(Mso::React::IReactInstance &reactInstance) noexcept;
  void ShowInstanceWaiting(Mso::React::IReactInstance &reactInstance) noexcept;
  void ShowInstanceLoaded(Mso::React::IReactInstance &reactInstance) noexcept;
  void ShowInstanceError() noexcept;

  void InitializeDeveloperMenu() noexcept;
  void ShowDeveloperMenu() noexcept;
  void DismissDeveloperMenu() noexcept;
  bool IsDeveloperMenuShowing() const noexcept;
  void ToggleInspector() noexcept;

  void ReloadHost() noexcept;
  void ReloadViewHost() noexcept;

  void AttachBackHandlers(XamlView const &rootView) noexcept;
  void RemoveBackHandlers() noexcept;
  bool OnBackRequested() noexcept;

 private:
  int64_t m_rootTag{-1};

  std::shared_ptr<TouchEventHandler> m_touchEventHandler;
  std::shared_ptr<SIPEventHandler> m_SIPEventHandler;
  std::shared_ptr<PreviewKeyboardEventHandlerOnRoot> m_previewKeyboardEventHandlerOnRoot;

  //  std::shared_ptr<IReactInstance> m_reactInstance;

  Mso::DispatchQueue m_uiQueue;
  Mso::CntPtr<Mso::React::IReactViewHost> m_reactViewHost;
  Mso::WeakPtr<Mso::React::IReactInstance> m_weakReactInstance;
  std::unique_ptr<Mso::React::ReactOptions> m_reactOptions;
  std::unique_ptr<Mso::React::ReactViewOptions> m_reactViewOptions;
  std::weak_ptr<facebook::react::InstanceWrapper> m_fbReactInstance;

  bool m_isDevModeEnabled{false};
  bool m_useFastRefresh{false};
  bool m_useWebDebugger{false};
  bool m_directDebugging{false};
  bool m_breakOnNextLine{false};
  bool m_isInitialized{false};
  bool m_isJSViewAttached{false};

  // Visual tree to support safe harbor
  // m_rootView
  //  safe harbor
  //  m_xamlRootView
  //    JS created children
  winrt::weak_ref<XamlView> m_weakRootView{nullptr};
  winrt::weak_ref<XamlView> m_weakXamlRootView{nullptr};

  winrt::ContentControl m_focusSafeHarbor{nullptr};
  winrt::ContentControl::LosingFocus_revoker m_focusSafeHarborLosingFocusRevoker{};
  winrt::Grid m_redBoxGrid{nullptr};
  winrt::Grid m_greenBoxGrid{nullptr};
  winrt::TextBlock m_errorTextBlock{nullptr};
  winrt::TextBlock m_waitingTextBlock{nullptr};
  winrt::Grid m_developerMenuRoot{nullptr};
  winrt::Button::Click_revoker m_remoteDebugJSRevoker{};
  winrt::Button::Click_revoker m_cancelRevoker{};
  winrt::Button::Click_revoker m_toggleInspectorRevoker{};
  winrt::Button::Click_revoker m_reloadJSRevoker{};
  winrt::Button::Click_revoker m_fastRefreshRevoker{};
  winrt::Button::Click_revoker m_directDebuggingRevoker{};
  winrt::Button::Click_revoker m_breakOnNextLineRevoker{};
  winrt::CoreDispatcher::AcceleratorKeyActivated_revoker m_coreDispatcherAKARevoker{};
  winrt::SystemNavigationManager::BackRequested_revoker m_backRequestedRevoker{};
};

//! This class ensures that we access ReactRootView from UI thread.
struct ReactViewInstance : public Mso::UnknownObject<Mso::RefCountStrategy::WeakRef, Mso::React::IReactViewInstance> {
  ReactViewInstance(std::weak_ptr<ReactRootControl> &&weakRootControl, Mso::DispatchQueue const &uiQueue) noexcept;

  Mso::Future<void> InitRootView(
      Mso::CntPtr<Mso::React::IReactInstance> &&reactInstance,
      Mso::React::ReactViewOptions &&viewOptions) noexcept override;
  Mso::Future<void> UpdateRootView() noexcept override;
  Mso::Future<void> UninitRootView() noexcept override;

 private:
  template <class TAction>
  Mso::Future<void> PostInUIQueue(TAction &&action) noexcept;

 private:
  std::weak_ptr<ReactRootControl> m_weakRootControl;
  Mso::DispatchQueue m_uiQueue;
};

//===========================================================================
// ReactViewInstance inline implementation
//===========================================================================

template <class TAction>
inline Mso::Future<void> ReactViewInstance::PostInUIQueue(TAction &&action) noexcept {
  // ReactViewInstance has shorter lifetime than ReactRootControl. Thus, we capture this WeakPtr.
  return Mso::PostFuture(
      m_uiQueue, [ weakThis = Mso::WeakPtr{this}, action{std::forward<TAction>(action)} ]() mutable noexcept {
        if (auto strongThis = weakThis.GetStrongPtr()) {
          if (auto rootControl = strongThis->m_weakRootControl.lock()) {
            action(*rootControl);
            return Mso::Maybe<void>{};
          }
        }

        return Mso::CancellationErrorProvider().MakeMaybe<void>();
      });
}

} // namespace react::uwp
