// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewProps.g.h"

#include <react/renderer/core/ConcreteState.h>
#include <react/renderer/core/State.h>
#include "winrt/Microsoft.ReactNative.h"

namespace Microsoft::ReactNative {

class AbiStateData {
 public:
  AbiStateData() = default;
  AbiStateData(winrt::Windows::Foundation::IInspectable data) : userdata(data) {}
  winrt::Windows::Foundation::IInspectable userdata;
};

template <typename TStateData /* = ::Microsoft::ReactNative::AbiStateData */>
struct AbiComponentState
    : winrt::implements<AbiComponentState<TStateData>, winrt::Microsoft::ReactNative::IComponentState> {
  AbiComponentState(facebook::react::State::Shared const &state) {
    m_state = std::static_pointer_cast<facebook::react::ConcreteState<TStateData> const>(state);
  }

  winrt::Windows::Foundation::IInspectable Data() noexcept {
    return m_state->getData().userdata;
  }
  void UpdateState(const winrt::Windows::Foundation::IInspectable &data) noexcept {
    m_state->updateState([data](const TStateData &oldData) {
      auto newData = oldData;
      newData.userdata = data;
      return std::make_shared<TStateData const>(std::move(newData));
    });
  }
  void UpdateStateWithMutation(const winrt::Microsoft::ReactNative::StateUpdateMutation &mutation) noexcept {
    m_state->updateState([mutation](const TStateData &oldData) {
      auto newData = oldData;
      newData.userdata = mutation(oldData.userdata);
      return std::make_shared<TStateData const>(std::move(newData));
    });
  }

 private:
  std::shared_ptr<facebook::react::ConcreteState<TStateData> const> m_state;
};

} // namespace Microsoft::ReactNative
