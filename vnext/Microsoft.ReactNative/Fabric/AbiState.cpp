// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "AbiState.h"

namespace Microsoft::ReactNative {

AbiComponentState::AbiComponentState(facebook::react::State::Shared const &state) {
  m_state = std::static_pointer_cast<facebook::react::ConcreteState<AbiStateData> const>(state);
}

winrt::Windows::Foundation::IInspectable AbiComponentState::Data() noexcept {
  return m_state->getData().userdata;
}

void AbiComponentState::UpdateState(const winrt::Windows::Foundation::IInspectable &data) noexcept {
  AbiStateData state;
  state.userdata = data;
  m_state->updateState(std::move(state));
}

void AbiComponentState::UpdateStateWithMutation(
    const winrt::Microsoft::ReactNative::StateUpdateMutation &mutation) noexcept {
  m_state->updateState([mutation](const AbiStateData &oldData) {
    return std::make_shared<AbiStateData const>(mutation(oldData.userdata));
  });
}

} // namespace Microsoft::ReactNative
