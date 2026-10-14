// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "AbiTextInputState.h"

namespace Microsoft::ReactNative {

AbiTextInputComponentState::AbiTextInputComponentState(facebook::react::State::Shared const &state) {
  m_state = std::static_pointer_cast<facebook::react::ConcreteState<AbiTextInputStateData> const>(state);
}

winrt::Windows::Foundation::IInspectable AbiTextInputComponentState::Data() noexcept {
  return m_state->getData().userdata;
}

void AbiTextInputComponentState::UpdateState(const winrt::Windows::Foundation::IInspectable &data) noexcept {
  // Maintaining the existing internal state fields, while replacing the userdata field with the new data.
  m_state->updateState([data](const AbiTextInputStateData &oldData) {
    return std::make_shared<AbiTextInputStateData const>(oldData.attributedStringBox, oldData.reactTreeAttributedString, oldData.paragraphAttributes, oldData.mostRecentEventCount, data);
  });
}

void AbiTextInputComponentState::UpdateStateWithMutation(
    const winrt::Microsoft::ReactNative::StateUpdateMutation &mutation) noexcept {
  m_state->updateState([mutation](const AbiTextInputStateData &oldData) {
    return std::make_shared<AbiTextInputStateData const>(mutation(oldData.userdata));
  });
}

} // namespace Microsoft::ReactNative
