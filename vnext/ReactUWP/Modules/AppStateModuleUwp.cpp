// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "AppStateModuleUwp.h"

#include <winrt/Windows.UI.Xaml.h>

namespace react {
namespace uwp {

//
// AppState
//

AppState::AppState(const std::shared_ptr<IReactInstance> &reactInstance)
    : facebook::react::AppState(), m_wkReactInstance(reactInstance) {
  m_lastState = "active";

  m_enteredBackgroundRevoker = winrt::Windows::UI::Xaml::Application::Current().EnteredBackground(
      winrt::auto_revoke, {this, &AppState::EnteredBackground});
  m_leavingBackgroundRevoker = winrt::Windows::UI::Xaml::Application::Current().LeavingBackground(
      winrt::auto_revoke, {this, &AppState::LeavingBackground});
}

AppState::~AppState() = default;

const char *AppState::getState() {
  return m_lastState;
}

void AppState::EnteredBackground(
    winrt::Windows::Foundation::IInspectable const & /*sender*/,
    winrt::Windows::ApplicationModel::EnteredBackgroundEventArgs const & /*e*/) {
  fireEvent("background");
}

void AppState::LeavingBackground(
    winrt::Windows::Foundation::IInspectable const & /*sender*/,
    winrt::Windows::ApplicationModel::LeavingBackgroundEventArgs const & /*e*/) {
  fireEvent("active");
}

void AppState::fireEvent(const char *newState) {
  auto instance = m_wkReactInstance.lock();
  if (instance) {
    m_lastState = newState;
    folly::dynamic parameters = folly::dynamic::object("app_state", newState);
    instance->CallJsFunction(
        "RCTDeviceEventEmitter", "emit", folly::dynamic::array("appStateDidChange", std::move(parameters)));
  }
}

} // namespace uwp
} // namespace react
