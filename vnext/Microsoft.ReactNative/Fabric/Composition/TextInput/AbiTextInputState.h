// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewProps.g.h"

#include <react/renderer/core/ConcreteState.h>
#include <react/renderer/core/State.h>
#include "WindowsTextInputState.h"
#include "winrt/Microsoft.ReactNative.h"

namespace Microsoft::ReactNative {

class AbiTextInputStateData : public facebook::react::WindowsTextInputState {
 public:
  AbiTextInputStateData() = default;
  // AbiTextInputStateData(winrt::Windows::Foundation::IInspectable data) : userdata(data) {}
  AbiTextInputStateData(
      facebook::react::AttributedStringBox attributedStringBox,
      facebook::react::AttributedString reactTreeAttributedString,
      facebook::react::ParagraphAttributes paragraphAttributes,
      int64_t mostRecentEventCount,
      winrt::Windows::Foundation::IInspectable userdata)
      : facebook::react::WindowsTextInputState(
            std::move(attributedStringBox),
            std::move(reactTreeAttributedString),
            std::move(paragraphAttributes),
            mostRecentEventCount),
        userdata(userdata) {}

  AbiTextInputStateData(winrt::Windows::Foundation::IInspectable userdata)
      : facebook::react::WindowsTextInputState({}), userdata(userdata) {}

  winrt::Windows::Foundation::IInspectable userdata;
};

struct AbiTextInputComponentState
    : winrt::implements<AbiTextInputComponentState, winrt::Microsoft::ReactNative::IComponentState> {
  AbiTextInputComponentState(facebook::react::State::Shared const &state);

  winrt::Windows::Foundation::IInspectable Data() noexcept;
  void UpdateState(const winrt::Windows::Foundation::IInspectable &data) noexcept;
  void UpdateStateWithMutation(const winrt::Microsoft::ReactNative::StateUpdateMutation &mutation) noexcept;

 private:
  std::shared_ptr<facebook::react::ConcreteState<AbiTextInputStateData> const> m_state;
};

} // namespace Microsoft::ReactNative
