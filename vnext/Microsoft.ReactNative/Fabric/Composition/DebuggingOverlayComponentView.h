
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompositionViewComponentView.h"

#include "Composition.DebuggingOverlayComponentView.g.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct DebuggingOverlayComponentView
    : DebuggingOverlayComponentViewT<DebuggingOverlayComponentView, CompositionViewComponentView> {
  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void mountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void unmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;

  DebuggingOverlayComponentView(
      const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

  void handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept override;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
