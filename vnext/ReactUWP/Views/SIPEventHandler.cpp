// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <pch.h>

#include "SIPEventHandler.h"

#include <Modules/NativeUIManager.h>

#include <ReactUWP/Utils/Helpers.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::ViewManagement::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Media;
} // namespace winrt
namespace react {
namespace uwp {

SIPEventHandler::SIPEventHandler(const std::weak_ptr<IReactInstance> &reactInstance)
    : m_wkReactInstance(reactInstance), m_fireKeyboradEvents(false), m_finalRect(winrt::RectHelper::Empty()){};

SIPEventHandler::~SIPEventHandler() {
  m_occlusionsChanged_revoker = {};
  m_loadedRevoker = {};
}
// keyboardDidHide and keyboardDidShow events works on >= RS3
// TryShow and TryHide works on >= RS5

void SIPEventHandler::AttachView(XamlView xamlView, bool fireKeyboardEvents) {
  m_fireKeyboradEvents = fireKeyboardEvents;
  // hookup CoreInputView only after element is in the tree
  m_view = winrt::make_weak(xamlView);
  if (winrt::VisualTreeHelper::GetParent(xamlView)) {
    InitializeCoreInputView();
  } else {
    m_loadedRevoker = xamlView.as<winrt::FrameworkElement>().Loaded(
        winrt::auto_revoke, [this](const auto & /*sender*/, const auto &) { InitializeCoreInputView(); });
  }
}

void SIPEventHandler::InitializeCoreInputView() {
  if (const auto xamlView = m_view.get()) {
    if (!IsRS3OrHigher()) {
      return; // CoreInputView is only supported on >= RS3.
    }

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
}
/*
void SIPEventHandler::TryShow() {
if (IsRS5OrHigher() && m_coreInputView && !m_isShowing) { // CoreInputView.TryShow is only avaliable after RS5
  m_coreInputView.TryShow();
}
}
*/

void SIPEventHandler::TryHide() {
  if (IsRS5OrHigher() && m_coreInputView && m_isShowing) { // CoreInputView.TryHide is only avaliable after RS5
    m_coreInputView.TryHide();
  }
}

bool SIPEventHandler::IsOcclusionsEmpty(winrt::IVectorView<winrt::CoreInputViewOcclusion> occlusions) {
  m_finalRect = winrt::RectHelper::Empty();
  if (occlusions) {
    for (const auto &occlusion : occlusions) {
      if (occlusion.OcclusionKind() == winrt::CoreInputViewOcclusionKind::Docked) {
        m_finalRect = winrt::RectHelper::Union(m_finalRect, occlusion.OccludingRect());
      }
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
