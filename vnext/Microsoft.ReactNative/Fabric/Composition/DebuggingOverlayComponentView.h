
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompositionViewComponentView.h"

#include "Composition.DebuggingOverlayComponentView.g.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct DebuggingOverlayComponentView
    : DebuggingOverlayComponentViewT<DebuggingOverlayComponentView, ViewComponentView> {
  [[nodiscard]] static winrt::Microsoft::ReactNative::ComponentView Create(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  void MountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;
  void UnmountChildComponentView(
      const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
      uint32_t index) noexcept override;

  DebuggingOverlayComponentView(
      const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
      facebook::react::Tag tag,
      winrt::Microsoft::ReactNative::ReactContext const &reactContext);

  void HandleCommand(const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept override;

 private:
  uint32_t m_activeOverlays{0};
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
