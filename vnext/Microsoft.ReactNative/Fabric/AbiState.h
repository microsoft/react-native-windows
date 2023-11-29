// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewProps.g.h"

#include <react/renderer/core/ConcreteState.h>
#include <react/renderer/core/State.h>
#include "winrt/Microsoft.ReactNative.h"

namespace Microsoft::ReactNative {

class AbiStateData final {
 public:
  AbiStateData() = default;
  AbiStateData(winrt::Windows::Foundation::IInspectable data) : userdata(data) {}
  winrt::Windows::Foundation::IInspectable userdata;
};

struct AbiComponentState : winrt::implements<AbiComponentState, winrt::Microsoft::ReactNative::IComponentState> {
  AbiComponentState(facebook::react::State::Shared const &state);

  winrt::Windows::Foundation::IInspectable Data() noexcept;
  void UpdateState(const winrt::Windows::Foundation::IInspectable &data) noexcept;
  void UpdateStateWithPriority(
      const winrt::Windows::Foundation::IInspectable &data,
      winrt::Microsoft::ReactNative::EventPriority priority) noexcept;
  void UpdateStateWithMutation(const winrt::Microsoft::ReactNative::StateUpdateMutation &mutation) noexcept;
  void UpdateStateWithMutationAndPriority(
      const winrt::Microsoft::ReactNative::StateUpdateMutation &mutation,
      winrt::Microsoft::ReactNative::EventPriority priority) noexcept;

 private:
  std::shared_ptr<facebook::react::ConcreteState<AbiStateData> const> m_state;
};

} // namespace Microsoft::ReactNative
