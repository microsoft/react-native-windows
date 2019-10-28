// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include "SIPEventHandler.h"

#include <Modules/NativeUIManager.h>
#include <Utils/RectUtils.h>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace winrt {
using namespace Windows::UI;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::ViewManagement::Core;
} // namespace winrt
namespace react {
namespace uwp {

SIPEventHandler::SIPEventHandler(const std::weak_ptr<IReactInstance> &reactInstance)
    : m_wkReactInstance(reactInstance) {
  auto coreInputView = winrt::CoreInputView::GetForCurrentView();
  if (coreInputView) {
    m_occlusionsChnaged_revoker = coreInputView.OcclusionsChanged(
        winrt::auto_revoke, [=](auto &&, const winrt::CoreInputViewOcclusionsChangedEventArgs &e) {
          if (!e.Handled()) {
            winrt::Rect finalRect = CreateEmptyRect();
            winrt::IVectorView<winrt::CoreInputViewOcclusion> occlusions = e.Occlusions();
            for (uint32_t i = 0; i < occlusions.Size(); i++) {
              winrt::CoreInputViewOcclusion occlusion = occlusions.GetAt(i);
              if (occlusion.OcclusionKind() == winrt::CoreInputViewOcclusionKind::Docked) {
                finalRect = UnionRect(finalRect, occlusion.OccludingRect());
              }
            }

            if (IsEmptyRect(finalRect)) {
              folly::dynamic params = folly::dynamic::object("screenY", 0)("screenX", 0)("width", 0)("height", 0);
              SendEvent("keyboardDidHide", std::move(params));
            } else {
              folly::dynamic params = folly::dynamic::object(
                  "endCoordinates",
                  folly::dynamic::object("screenY", finalRect.Y)("screenX", finalRect.X)("width", finalRect.Width)(
                      "height", finalRect.Height));
              SendEvent("keyboardDidShow", std::move(params));
            }
          }
        });
  }
}

SIPEventHandler::~SIPEventHandler() {
  m_occlusionsChnaged_revoker = {};
}

void SIPEventHandler::SendEvent(std::string &&eventName, folly::dynamic &&parameters) {
  if (auto instance = m_wkReactInstance.lock()) {
    instance->CallJsFunction(
        "RCTDeviceEventEmitter", "emit", folly::dynamic::array(std::move(eventName), std::move(parameters)));
  }
}
} // namespace uwp
} // namespace react
