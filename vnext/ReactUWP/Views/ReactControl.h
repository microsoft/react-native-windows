// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <stdint.h>
#include <memory>
#include <set>

#include <IReactInstance.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include "IXamlRootView.h"
#include "SIPEventHandler.h"
#include "TouchEventHandler.h"
#include "Views/KeyboardEventHandler.h"

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt

namespace react {
namespace uwp {

class ReactControl final : public std::enable_shared_from_this<ReactControl>, public IXamlReactControl {
 public:
  ReactControl(IXamlRootView *parent, XamlView rootView);

  virtual ~ReactControl();

 public: // IXamlRootView
  std::shared_ptr<IReactInstance> GetReactInstance() const noexcept;
  XamlView GetXamlView() const noexcept;

  void SetJSComponentName(std::string &&mainComponentName) noexcept;
  void SetInstanceCreator(const ReactInstanceCreator &instanceCreator) noexcept;
  void SetInitialProps(folly::dynamic &&initialProps) noexcept;

  void AttachRoot() noexcept;
  void DetachRoot() noexcept;

 public: // IXamlReactControl
  void blur(XamlView const &xamlView) noexcept override;

  void DetachInstance();
  void Reload(bool shouldRetireCurrentInstance);

  void ResetView() {}
  virtual std::string JSComponentName() const noexcept;
  virtual int64_t GetActualHeight() const;
  virtual int64_t GetActualWidth() const;
  int64_t GetTag() const {
    return m_rootTag;
  }
  void SetTag(int64_t tag) {
    m_rootTag = tag;
  }

 private:
  void HandleInstanceError();
  void HandleInstanceErrorOnUIThread();
  void HandleInstanceWaiting();
  void HandleInstanceWaitingOnUIThread();
  void HandleDebuggerAttach();
  void HandleDebuggerAttachOnUIThread();
  void PrepareXamlRootView(XamlView const &rootView);
  void EnsureFocusSafeHarbor();
  void InitializeDeveloperMenu();
  void ShowDeveloperMenu();
  void DismissDeveloperMenu();
  bool IsDeveloperMenuShowing() const;
  void ToggleInspector();

  IXamlRootView *m_pParent;

  std::string m_jsComponentName;
  std::shared_ptr<facebook::react::NativeModuleProvider> m_moduleProvider;
  folly::dynamic m_initialProps;
  std::shared_ptr<TouchEventHandler> m_touchEventHandler;
  std::shared_ptr<SIPEventHandler> m_SIPEventHandler;
  std::shared_ptr<PreviewKeyboardEventHandlerOnRoot> m_previewKeyboardEventHandlerOnRoot;

  int64_t m_rootTag = -1;

  // Visual tree to support safe harbor
  // m_rootView
  //  safe harbor
  //  m_xamlRootView
  //    JS created children
  XamlView m_xamlRootView{nullptr};
  XamlView m_rootView;

  ReactInstanceCreator m_instanceCreator;
  std::shared_ptr<IReactInstance> m_reactInstance;
  bool m_isAttached{false};
  LiveReloadCallbackCookie m_liveReloadCallbackCookie{0};
  ErrorCallbackCookie m_errorCallbackCookie{0};
  DebuggerAttachCallbackCookie m_debuggerAttachCallbackCookie{0};

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
  winrt::Button::Click_revoker m_liveReloadRevoker{};
  winrt::Button::Click_revoker m_directDebuggingRevoker{};
  winrt::Button::Click_revoker m_breakOnNextLineRevoker{};
  winrt::Windows::UI::Core::CoreDispatcher m_uiDispatcher;
  winrt::CoreDispatcher::AcceleratorKeyActivated_revoker m_coreDispatcherAKARevoker{};
};

} // namespace uwp
} // namespace react
