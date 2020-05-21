// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>

namespace Microsoft::ReactNative {

REACT_MODULE(AppState)
struct AppState: public std::enable_shared_from_this<AppState> {
  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(getCurrentAppState)
  void getCurrentAppState(std::function<void(React::JSValue const &)> const & success, std::function<void(React::JSValue const &)> const & error) noexcept;

  REACT_METHOD(addListener)
  void addListener(std::string eventName) noexcept;

  REACT_METHOD(removeListeners)
  void removeListeners(double count) noexcept;

  REACT_CONSTANT(initialAppState)
  const std::string initialAppState{"active"};

private:
  void SetActive(bool active) noexcept;

  std::mutex m_stateMutex;
  std::atomic<bool> m_active;
  char const *m_lastState{nullptr};
  winrt::Microsoft::ReactNative::ReactContext m_context;
  xaml::Application::EnteredBackground_revoker m_enteredBackgroundRevoker;
  xaml::Application::LeavingBackground_revoker m_leavingBackgroundRevoker;
};

} // namespace Microsoft::ReactNative
