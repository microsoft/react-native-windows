// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "codegen/NativeAppStateSpec.g.h"
#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>

#include <winrt/Windows.UI.Core.h>

namespace Microsoft::ReactNative {

REACT_MODULE(AppState)
struct AppState : public std::enable_shared_from_this<AppState> {
  using ModuleSpec = ReactNativeSpecs::AppStateSpec;
  using AppStateChangeArgs = ReactNativeSpecs::AppStateSpec_getCurrentAppState_success_appState;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(GetCurrentAppState, L"getCurrentAppState")
  void GetCurrentAppState(
      std::function<void(AppStateChangeArgs const &)> const &success,
      std::function<void(React::JSValue const &)> const &error) noexcept;

  REACT_METHOD(AddListener, L"addListener")
  void AddListener(std::string &&eventName) noexcept;

  REACT_METHOD(RemoveListeners, L"removeListeners")
  void RemoveListeners(double count) noexcept;

  REACT_GET_CONSTANTS(GetConstants)
  ReactNativeSpecs::AppStateSpec_Constants GetConstants() noexcept;

  REACT_EVENT(AppStateDidChange, L"appStateDidChange")
  std::function<void(AppStateChangeArgs const &)> AppStateDidChange;

 private:
  void SetDeactivated(bool deactivated) noexcept;
  void SetEnteredBackground(bool enteredBackground) noexcept;
  std::string GetAppState() noexcept;

  std::mutex m_stateMutex;
  std::atomic<bool> m_deactivated;
  std::atomic<bool> m_enteredBackground;
  char const *m_lastState{nullptr};
  React::ReactContext m_context;
#ifndef USE_WINUI3
  xaml::Application::EnteredBackground_revoker m_enteredBackgroundRevoker;
  xaml::Application::LeavingBackground_revoker m_leavingBackgroundRevoker;
  winrt::Windows::UI::Core::CoreWindow::Activated_revoker m_activatedEventRevoker;
#endif
};

} // namespace Microsoft::ReactNative
