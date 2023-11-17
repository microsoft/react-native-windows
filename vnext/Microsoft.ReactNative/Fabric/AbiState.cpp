// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "AbiState.h"

namespace Microsoft::ReactNative {

AbiComponentState::AbiComponentState(facebook::react::State::Shared const &state) {
  m_state = std::static_pointer_cast<facebook::react::ConcreteState<AbiStateData> const>(state);
}

winrt::IInspectable AbiComponentState::Data() noexcept {
  return m_state->getData().userdata;
}

void AbiComponentState::UpdateState(winrt::IInspectable data) noexcept {
  AbiStateData state;
  state.userdata = data;
  m_state->updateState(std::move(state));
}

} // namespace Microsoft::ReactNative
