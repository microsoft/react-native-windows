
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "DebuggingOverlayComponentView.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

DebuggingOverlayComponentView::DebuggingOverlayComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext)
    : base_type(compContext, tag, reactContext) {}

void DebuggingOverlayComponentView::mountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

void DebuggingOverlayComponentView::unmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  assert(false);
}

winrt::Microsoft::ReactNative::ComponentView DebuggingOverlayComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<DebuggingOverlayComponentView>(compContext, tag, reactContext);
}

void DebuggingOverlayComponentView::handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept {
  if (commandName == "draw") {
    // The current spec has a Draw command -- but this will be replaced with some different commands in
    // https://github.com/facebook/react-native/pull/42119
    // There is little point in attempting to implement these commands until then.
    return;
  }

  base_type::handleCommand(commandName, arg);
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
