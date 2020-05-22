// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AppStateModule.h"
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "Unicode.h"

namespace Microsoft::ReactNative {

void AppState::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
  m_active = true;

  auto currentApp = xaml::Application::Current();

  m_enteredBackgroundRevoker = currentApp.EnteredBackground(
      winrt::auto_revoke,
      [weakThis = weak_from_this()](
          winrt::IInspectable const & /*sender*/,
          winrt::Windows::ApplicationModel::EnteredBackgroundEventArgs const & /*e*/) noexcept {
        if (auto strongThis = weakThis.lock()) {
          strongThis->SetActive(false);
        }
      });

  m_leavingBackgroundRevoker = currentApp.LeavingBackground(
      winrt::auto_revoke,
      [weakThis = weak_from_this()](
          winrt::IInspectable const & /*sender*/,
          winrt::Windows::ApplicationModel::LeavingBackgroundEventArgs const & /*e*/) noexcept {
        if (auto strongThis = weakThis.lock()) {
          strongThis->SetActive(true);
        }
      });
}

void AppState::getCurrentAppState(
    std::function<void(React::JSValue const &)> const &success,
    std::function<void(React::JSValue const &)> const &error) noexcept {
  success(m_active ? "active" : "background");
}

void AppState::addListener(std::string eventName) noexcept {
  // noop
}

void AppState::removeListeners(double count) noexcept {
  // noop
}

void AppState::SetActive(bool active) noexcept {
  m_active = active;

  m_context.EmitJSEvent(
      L"RCTDeviceEventEmitter",
      L"appStateDidChange",
      React::JSValueObject{{"app_state", m_active ? "active" : "background"}});
}

} // namespace Microsoft::ReactNative
