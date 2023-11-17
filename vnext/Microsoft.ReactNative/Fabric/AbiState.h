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
  winrt::IInspectable userdata;
};

struct AbiComponentState : winrt::implements<AbiComponentState, winrt::Microsoft::ReactNative::IComponentState> {
  AbiComponentState(facebook::react::State::Shared const &state);

  winrt::IInspectable Data() noexcept;
  void UpdateState(winrt::IInspectable data) noexcept;

 private:
  std::shared_ptr<facebook::react::ConcreteState<AbiStateData> const> m_state;
};

} // namespace Microsoft::ReactNative
