
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "DebuggingOverlayComponentView.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

DebuggingOverlayComponentView::DebuggingOverlayComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : base_type(
          compContext,
          tag,
          reactContext,
          ComponentViewFeatures::Default &
              ~(ComponentViewFeatures::Background | ComponentViewFeatures::ShadowProps |
                ComponentViewFeatures::NativeBorder),
          false) {}

void DebuggingOverlayComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::MountChildComponentView(childComponentView, index);
}

void DebuggingOverlayComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
  base_type::UnmountChildComponentView(childComponentView, index);
}

winrt::Microsoft::ReactNative::ComponentView DebuggingOverlayComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<DebuggingOverlayComponentView>(compContext, tag, reactContext);
}

void DebuggingOverlayComponentView::HandleCommand(
    winrt::hstring commandName,
    const winrt::Microsoft::ReactNative::IJSValueReader &args) noexcept {
  if (commandName == L"draw") {
    // The current spec has a Draw command -- but this will be replaced with some different commands in
    // https://github.com/facebook/react-native/pull/42119
    // There is little point in attempting to implement these commands until then.
    return;
  }

  base_type::HandleCommand(commandName, args);
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
