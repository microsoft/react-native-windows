// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AppStateData.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel;
using namespace Windows::UI::Xaml;

namespace react::uwp {

AppStateData::AppStateData(Mso::React::IReactContext &reactContext, Mso::DispatchQueue const &uiQueue) noexcept
    : Super(uiQueue), m_lastState{"active"}, m_reactContext{&reactContext} {}

AppStateData::~AppStateData() = default;

void AppStateData::Initialize() noexcept {
  auto currentApp = Application::Current();

  m_enteredBackgroundRevoker = currentApp.EnteredBackground(
      winrt::auto_revoke,
      [weakThis = Mso::WeakPtr{this}](
          IInspectable const & /*sender*/, EnteredBackgroundEventArgs const & /*e*/) noexcept {
        if (auto strongThis = weakThis.GetStrongPtr()) {
          strongThis->RaiseEvent("background");
        }
      });

  m_leavingBackgroundRevoker = currentApp.LeavingBackground(
      winrt::auto_revoke,
      [weakThis = Mso::WeakPtr{this}](
          IInspectable const & /*sender*/, LeavingBackgroundEventArgs const & /*e*/) noexcept {
        if (auto strongThis = weakThis.GetStrongPtr()) {
          strongThis->RaiseEvent("active");
        }
      });
}

void AppStateData::Finalize() noexcept {
  m_enteredBackgroundRevoker = {};
  m_leavingBackgroundRevoker = {};
}

char const *AppStateData::GetState() noexcept {
  std::lock_guard lock{m_stateMutex};
  return m_lastState;
}

void AppStateData::RaiseEvent(char const *newState) noexcept {
  {
    std::lock_guard lock{m_stateMutex};
    m_lastState = newState;
  }

  folly::dynamic parameters = folly::dynamic::object("app_state", newState);
  m_reactContext->CallJSFunction(
      "RCTDeviceEventEmitter", "emit", folly::dynamic::array("appStateDidChange", std::move(parameters)));
}

AppState2::AppState2(Mso::React::IReactContext &reactContext, Mso::DispatchQueue const &uiQueue) noexcept
    : m_data{Mso::Make<AppStateData>(reactContext, uiQueue)} {}

AppState2::~AppState2() = default;

char const *AppState2::getState() {
  return m_data->GetState();
}

} // namespace react::uwp
