// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>

#include <React.h>
#include <winrt/Windows.UI.ViewManagement.Core.h>

namespace winrt {
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::ViewManagement::Core;
} // namespace winrt

namespace Microsoft::ReactNative {

class SIPEventHandler {
 public:
  SIPEventHandler(const Mso::React::IReactContext &context);
  virtual ~SIPEventHandler();

  bool IsSIPShowing() {
    return m_isShowing;
  }

  void AttachView(XamlView xamlView, bool fireKeyboardEvents);
  // void TryShow();
  void TryHide();

 private:
  bool IsOcclusionsEmpty(winrt::IVectorView<winrt::CoreInputViewOcclusion> occlusions);
  void SendEvent(std::string &&eventName, folly::dynamic &&parameters);
  Mso::CntPtr<const Mso::React::IReactContext> m_context;
  winrt::CoreInputView::OcclusionsChanged_revoker m_occlusionsChanged_revoker;
  winrt::Rect m_finalRect;
  winrt::CoreInputView m_coreInputView{nullptr};
  winrt::weak_ref<XamlView> m_view{};
  xaml::FrameworkElement::Loaded_revoker m_loadedRevoker{};
  bool m_isShowing{false};
  bool m_fireKeyboradEvents;

  void InitializeCoreInputView();
};

} // namespace Microsoft::ReactNative
