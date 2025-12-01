// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "AppStateModule.h"
#include <QuirkSettings.h>
#include <Utils/Helpers.h>
#include <XamlUtils.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "Unicode.h"

using namespace winrt::Windows::UI::Core;

namespace Microsoft::ReactNative {

void AppState::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
  m_deactivated = false;
  m_enteredBackground = false;
}

void AppState::GetCurrentAppState(
    std::function<void(AppStateChangeArgs const &)> const &success,
    std::function<void(React::JSValue const &)> const &error) noexcept {
  AppStateChangeArgs args;
  args.app_state = GetAppState();
  success(args);
}

void AppState::AddListener(std::string && /*eventName*/) noexcept {
  // noop
}

void AppState::RemoveListeners(double /*count*/) noexcept {
  // noop
}

ReactNativeSpecs::AppStateSpec_AppStateConstants AppState::GetConstants() noexcept {
  return {GetAppState()};
}

void AppState::SetDeactivated(bool deactivated) noexcept {
  if (winrt::Microsoft::ReactNative::implementation::QuirkSettings::GetMapWindowDeactivatedToAppStateInactive(
          m_context.Properties())) {
    m_deactivated = deactivated;
    AppStateDidChange({GetAppState()});
  }
}

void AppState::SetEnteredBackground(bool enteredBackground) noexcept {
  m_enteredBackground = enteredBackground;
  AppStateDidChange({GetAppState()});
}

std::string AppState::GetAppState() noexcept {
  return m_enteredBackground ? "background" : (m_deactivated ? "inactive" : "active");
}

} // namespace Microsoft::ReactNative