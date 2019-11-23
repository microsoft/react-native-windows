// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include "SIPEventHandler.h"

#include <Modules/NativeUIManager.h>

#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Foundation.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::ViewManagement::Core;
using namespace Windows::UI::Xaml;
} // namespace winrt
namespace react {
namespace uwp {

SIPEventHandler::SIPEventHandler(const std::weak_ptr<IReactInstance> &reactInstance)
    : m_wkReactInstance(reactInstance) {
  auto coreInputView = winrt::CoreInputView::GetForCurrentView();
  if (coreInputView) {
    m_occlusionsChanged_revoker = coreInputView.OcclusionsChanged(
        winrt::auto_revoke, [=](auto &&, const winrt::CoreInputViewOcclusionsChangedEventArgs &e) {
          if (!e.Handled()) {
            winrt::Rect finalRect = winrt::RectHelper::Empty();
            winrt::IVectorView<winrt::CoreInputViewOcclusion> occlusions = e.Occlusions();
            for (uint32_t i = 0; i < occlusions.Size(); i++) {
              winrt::CoreInputViewOcclusion occlusion = occlusions.GetAt(i);
              if (occlusion.OcclusionKind() == winrt::CoreInputViewOcclusionKind::Docked) {
                finalRect = winrt::RectHelper::Union(finalRect, occlusion.OccludingRect());
              }
            }

            if (winrt::RectHelper::GetIsEmpty(finalRect)) {
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
  m_occlusionsChanged_revoker = {};
}

void SIPEventHandler::SendEvent(std::string &&eventName, folly::dynamic &&parameters) {
  if (auto instance = m_wkReactInstance.lock()) {
    instance->CallJsFunction(
        "RCTDeviceEventEmitter", "emit", folly::dynamic::array(std::move(eventName), std::move(parameters)));
  }
}
} // namespace uwp
} // namespace react
