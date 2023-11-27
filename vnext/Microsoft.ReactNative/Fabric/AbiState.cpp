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

// Verify winrt::Microsoft::ReactNative::EventPriority and facebook::react::EventPriority are in sync
static_assert(
    winrt::Microsoft::ReactNative::EventPriority::SynchronousUnbatched ==
    static_cast<winrt::Microsoft::ReactNative::EventPriority>(facebook::react::EventPriority::SynchronousUnbatched));
static_assert(
    winrt::Microsoft::ReactNative::EventPriority::SynchronousBatched ==
    static_cast<winrt::Microsoft::ReactNative::EventPriority>(facebook::react::EventPriority::SynchronousBatched));
static_assert(
    winrt::Microsoft::ReactNative::EventPriority::AsynchronousUnbatched ==
    static_cast<winrt::Microsoft::ReactNative::EventPriority>(facebook::react::EventPriority::AsynchronousUnbatched));
static_assert(
    winrt::Microsoft::ReactNative::EventPriority::AsynchronousBatched ==
    static_cast<winrt::Microsoft::ReactNative::EventPriority>(facebook::react::EventPriority::AsynchronousBatched));
static_assert(
    winrt::Microsoft::ReactNative::EventPriority::Sync ==
    static_cast<winrt::Microsoft::ReactNative::EventPriority>(facebook::react::EventPriority::Sync));
static_assert(
    winrt::Microsoft::ReactNative::EventPriority::Work ==
    static_cast<winrt::Microsoft::ReactNative::EventPriority>(facebook::react::EventPriority::Work));
static_assert(
    winrt::Microsoft::ReactNative::EventPriority::Interactive ==
    static_cast<winrt::Microsoft::ReactNative::EventPriority>(facebook::react::EventPriority::Interactive));
static_assert(
    winrt::Microsoft::ReactNative::EventPriority::Deferred ==
    static_cast<winrt::Microsoft::ReactNative::EventPriority>(facebook::react::EventPriority::Deferred));

void AbiComponentState::UpdateStateWithPriority(
    const winrt::Windows::Foundation::IInspectable &data,
    winrt::Microsoft::ReactNative::EventPriority priority) noexcept {
  AbiStateData state;
  state.userdata = data;
  m_state->updateState(std::move(state), static_cast<facebook::react::EventPriority>(priority));
}

void AbiComponentState::UpdateStateWithMutation(
    const winrt::Microsoft::ReactNative::StateUpdateMutation &mutation) noexcept {
  UpdateStateWithMutationAndPriority(mutation, winrt::Microsoft::ReactNative::EventPriority::AsynchronousUnbatched);
}

void AbiComponentState::UpdateStateWithMutationAndPriority(
    const winrt::Microsoft::ReactNative::StateUpdateMutation &mutation,
    winrt::Microsoft::ReactNative::EventPriority priority) noexcept {
  m_state->updateState(
      [mutation](const AbiStateData &oldData) {
        return std::make_shared<AbiStateData const>(mutation(oldData.userdata));
      },
      static_cast<facebook::react::EventPriority>(priority));
}

} // namespace Microsoft::ReactNative
