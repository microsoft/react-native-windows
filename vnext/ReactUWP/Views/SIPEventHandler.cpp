// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include "SIPEventHandler.h"

#include <Modules/NativeUIManager.h>

#include <ReactUWP\Utils\Helpers.h>
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
    : m_wkReactInstance(reactInstance), m_fireKeyboradEvents(false){};

SIPEventHandler::~SIPEventHandler() {
  m_occlusionsChanged_revoker = {};
}

void SIPEventHandler::AttachView(XamlView xamlView, bool fireKeyboardEvents) {
  m_fireKeyboradEvents = fireKeyboardEvents;

  if (Is19H1OrHigher()) {
    // 19H1 and higher supports island scenarios
    auto uiElement(xamlView.as<winrt::UIElement>());
    m_coreInputView = winrt::CoreInputView::GetForUIContext(uiElement.UIContext());
  } else {
    m_coreInputView = winrt::CoreInputView::GetForCurrentView();
  }

  if (m_coreInputView) {
    auto occlusions = m_coreInputView.GetCoreInputViewOcclusions();
    m_isShowing = !IsOcclusionsEmpty(occlusions);
    m_occlusionsChanged_revoker = m_coreInputView.OcclusionsChanged(
        winrt::auto_revoke, [this](auto &&, const winrt::CoreInputViewOcclusionsChangedEventArgs &e) {
          if (!e.Handled()) {
            bool wasShowing = m_isShowing;
            m_isShowing = !IsOcclusionsEmpty(e.Occlusions());
            if (wasShowing != m_isShowing && m_fireKeyboradEvents) {
              if (!m_isShowing) {
                folly::dynamic params = folly::dynamic::object("screenY", 0)("screenX", 0)("width", 0)("height", 0);
                SendEvent("keyboardDidHide", std::move(params));
              } else {
                folly::dynamic params = folly::dynamic::object(
                    "endCoordinates",
                    folly::dynamic::object("screenY", m_finalRect.Y)("screenX", m_finalRect.X)(
                        "width", m_finalRect.Width)("height", m_finalRect.Height));
                SendEvent("keyboardDidShow", std::move(params));
              }
            }
          }
        });
  }
}

bool SIPEventHandler::TryShow() {
  if (m_coreInputView == nullptr) {
    return false;
  }
  if (!m_isShowing) {
    return m_coreInputView.TryShow();
  }
  return true;
}

bool SIPEventHandler::TryHide() {
  if (m_coreInputView == nullptr) {
    return false;
  }
  if (m_isShowing) {
    return m_coreInputView.TryHide();
  }
  return true;
}

bool SIPEventHandler::IsOcclusionsEmpty(winrt::IVectorView<winrt::CoreInputViewOcclusion> occlusions) {
  m_finalRect = winrt::RectHelper::Empty();

  for (uint32_t i = 0; i < occlusions.Size(); i++) {
    winrt::CoreInputViewOcclusion occlusion = occlusions.GetAt(i);
    if (occlusion.OcclusionKind() == winrt::CoreInputViewOcclusionKind::Docked) {
      m_finalRect = winrt::RectHelper::Union(m_finalRect, occlusion.OccludingRect());
    }
  }

  return (winrt::RectHelper::GetIsEmpty(m_finalRect));
}

void SIPEventHandler::SendEvent(std::string &&eventName, folly::dynamic &&parameters) {
  if (auto instance = m_wkReactInstance.lock()) {
    instance->CallJsFunction(
        "RCTDeviceEventEmitter", "emit", folly::dynamic::array(std::move(eventName), std::move(parameters)));
  }
}
} // namespace uwp
} // namespace react
