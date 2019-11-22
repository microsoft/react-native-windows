// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>

#include <IReactInstance.h>
#include <winrt/Windows.UI.ViewManagement.Core.h>

namespace winrt {
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::ViewManagement::Core;
} // namespace winrt

namespace react {
namespace uwp {

class SIPEventHandler {
 public:
  SIPEventHandler(const std::weak_ptr<IReactInstance> &reactInstance);
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
  std::weak_ptr<IReactInstance> m_wkReactInstance;
  winrt::CoreInputView::OcclusionsChanged_revoker m_occlusionsChanged_revoker;
  winrt::Rect m_finalRect;
  winrt::CoreInputView m_coreInputView{nullptr};
  bool m_isShowing{false};
  bool m_fireKeyboradEvents;
};

} // namespace uwp
} // namespace react
