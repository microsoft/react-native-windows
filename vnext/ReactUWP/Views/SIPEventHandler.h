// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <folly/dynamic.h>

#include <IReactInstance.h>
#include <winrt/Windows.UI.ViewManagement.Core.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI::ViewManagement::Core;
} // namespace winrt

namespace react {
namespace uwp {

class SIPEventHandler {
 public:
  SIPEventHandler(const std::weak_ptr<IReactInstance> &reactInstance);
  virtual ~SIPEventHandler();

 private:
  void SendEvent(std::string &&eventName, folly::dynamic &&parameters);
  std::weak_ptr<IReactInstance> m_wkReactInstance;
  winrt::CoreInputView::OcclusionsChanged_revoker m_occlusionsChnaged_revoker;
};

} // namespace uwp
} // namespace react
