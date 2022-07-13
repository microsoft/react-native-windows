// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "AppStateModule.h"
#include <Utils/Helpers.h>
#include <XamlUtils.h>
#include <winrt/Windows.ApplicationModel.DataTransfer.h>
#include "Unicode.h"
#include <QuirkSettings.h>

using namespace winrt::Windows::UI::Core;

namespace Microsoft::ReactNative {

void AppState::Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
  m_deactived = false;
  m_enteredBackground = false;

  // We need to register for notifications from the XAML thread.
  if (auto dispatcher = reactContext.UIDispatcher()) {
    dispatcher.Post([this]() {
      auto currentApp = xaml::TryGetCurrentApplication();

      if (!IsWinUI3Island() && currentApp != nullptr) {
#ifndef USE_WINUI3
        CoreWindow window = CoreWindow::GetForCurrentThread();

        m_enteredBackgroundRevoker = currentApp.EnteredBackground(
            winrt::auto_revoke,
            [weakThis = weak_from_this()](
                winrt::IInspectable const & /*sender*/,
                winrt::Windows::ApplicationModel::EnteredBackgroundEventArgs const & /*e*/) noexcept {
              if (auto strongThis = weakThis.lock()) {
                strongThis->SetEnteredBackground(true);
              }
            });

        m_leavingBackgroundRevoker = currentApp.LeavingBackground(
            winrt::auto_revoke,
            [weakThis = weak_from_this()](
                winrt::IInspectable const & /*sender*/,
                winrt::Windows::ApplicationModel::LeavingBackgroundEventArgs const & /*e*/) noexcept {
              if (auto strongThis = weakThis.lock()) {
                strongThis->SetEnteredBackground(false);
              }
            });
        if (window != nullptr &&
            winrt::Microsoft::ReactNative::implementation::QuirkSettings::GetMapWindowDeactivatedToAppStateInactive(m_context.Properties())) {
          m_activatedEventRevoker = window.Activated(
              winrt::auto_revoke,
              [weakThis = weak_from_this()](
                  winrt::Windows::UI::Core::CoreWindow /*sender*/,
                  winrt::Windows::UI::Core::WindowActivatedEventArgs args) {
                if (auto strongThis = weakThis.lock()) {
                  if (args.WindowActivationState() ==
                      winrt::Windows::UI::Core::CoreWindowActivationState::Deactivated) {
                    strongThis->SetDeactived(true);
                  } else {
                    strongThis->SetDeactived(false);
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
  args.app_state = m_enteredBackground ? "background" : m_deactived ? "inactive" : "active";
  success(args);
}

void AppState::AddListener(std::string && /*eventName*/) noexcept {
  // noop
}

void AppState::RemoveListeners(double /*count*/) noexcept {
  // noop
}

ReactNativeSpecs::AppStateSpec_Constants AppState::GetConstants() noexcept {
  return {m_enteredBackground ? "background" : m_deactived ? "inactive" : "active"};
}

void AppState::SetDeactived(bool deactived) noexcept {
  if (winrt::Microsoft::ReactNative::implementation::QuirkSettings::GetMapWindowDeactivatedToAppStateInactive(m_context.Properties())){
    
      m_deactived = deactived;
    m_context.JSDispatcher().Post([this]() {
      AppStateDidChange({m_enteredBackground ? "background" : m_deactived ? "inactive" : "active"});
    });
  }
}

void AppState::SetEnteredBackground(bool enteredBackground) noexcept {
  m_enteredBackground = enteredBackground;
  m_context.JSDispatcher().Post([this]() {
    AppStateDidChange({m_enteredBackground ? "background" : m_deactived ? "inactive" : "active"});
  });
}

} // namespace Microsoft::ReactNative