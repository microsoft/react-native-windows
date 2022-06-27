// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "AppStateModule.h"
#include <Utils/Helpers.h>
#include <XamlUtils.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "Unicode.h"

using namespace winrt::Windows::UI::Core;

namespace Microsoft::ReactNative {

void AppState::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
  m_active = true;

  // We need to register for notifications from the XAML thread.
  if (auto dispatcher = reactContext.UIDispatcher()) {
    dispatcher.Post([this]() {
      if (!IsWinUI3Island()) {
#ifndef USE_WINUI3
        CoreWindow window = CoreWindow::GetForCurrentThread();
        if (window != nullptr) {
          m_activatedEventRevoker = window.Activated(
              winrt::auto_revoke,
              [weakThis = weak_from_this()](
                  winrt::Windows::UI::Core::CoreWindow /*sender*/,
                  winrt::Windows::UI::Core::WindowActivatedEventArgs args) {
                if (auto strongThis = weakThis.lock()) {
                  if (args.WindowActivationState() ==
                      winrt::Windows::UI::Core::CoreWindowActivationState::Deactivated) {
                    strongThis->SetActive(false);
                  } else {
                    strongThis->SetActive(true);
                  }
                }
              });
        }
#endif
      } else {
        assert(IsXamlIsland());
      }
    });
  }
}

void AppState::GetCurrentAppState(
    std::function<void(AppStateChangeArgs const &)> const &success,
    std::function<void(React::JSValue const &)> const &error) noexcept {
  AppStateChangeArgs args;
  args.app_state = m_active ? "active" : "background";
  success(args);
}

void AppState::AddListener(std::string && /*eventName*/) noexcept {
  // noop
}

void AppState::RemoveListeners(double /*count*/) noexcept {
  // noop
}

ReactNativeSpecs::AppStateSpec_Constants AppState::GetConstants() noexcept {
  return {m_active ? "active" : "background"};
}

void AppState::SetActive(bool active) noexcept {
  m_active = active;
  m_context.JSDispatcher().Post([this]() { AppStateDidChange({m_active ? "active" : "background"}); });
}

} // namespace Microsoft::ReactNative
