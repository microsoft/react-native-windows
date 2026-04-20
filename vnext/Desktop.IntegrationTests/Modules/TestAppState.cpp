// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "TestAppState.h"

namespace Microsoft::React::Test {

void AppState::Initialize(winrt::Microsoft::ReactNative::ReactContext const &) noexcept {}

void AppState::GetCurrentAppState(
    std::function<void(::Microsoft::ReactNativeSpecs::AppStateSpec_AppState const &)> const &success,
    std::function<void(winrt::Microsoft::ReactNative::JSValue const &)> const &) noexcept {
  success({.app_state = "active"});
}

void AppState::AddListener(std::string) noexcept {}

void AppState::RemoveListeners(double) noexcept {}

::Microsoft::ReactNativeSpecs::AppStateSpec_AppStateConstants AppState::GetConstants() noexcept {
  return {.initialAppState = "active"};
}

} // namespace Microsoft::React::Test
