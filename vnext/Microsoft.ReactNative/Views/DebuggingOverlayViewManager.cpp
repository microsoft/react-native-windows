// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Xaml.Controls.h>

#include <Views/DebuggingOverlayViewManager.h>

#include <JSValueWriter.h>

namespace Microsoft::ReactNative {

namespace DebuggingOverlayCommands {
constexpr const char *Draw = "draw";
}; // namespace DebuggingOverlayCommands

DebuggingOverlayViewManager::DebuggingOverlayViewManager(const Mso::React::IReactContext &context) : Super(context) {}

const wchar_t *DebuggingOverlayViewManager::GetName() const {
  return L"DebuggingOverlay";
}

void DebuggingOverlayViewManager::GetNativeProps(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  Super::GetNativeProps(writer);
}

void DebuggingOverlayViewManager::GetCommands(const winrt::Microsoft::ReactNative::IJSValueWriter &writer) const {
  // Upstream JS will dispatch the string directly instead of ever actually calling this, but providing a real
  // implementation is simple enough in case anything changes.
  writer.WritePropertyName(winrt::to_hstring(DebuggingOverlayCommands::Draw));
  writer.WriteString(winrt::to_hstring(DebuggingOverlayCommands::Draw));
}

XamlView DebuggingOverlayViewManager::CreateViewCore(
    int64_t /*tag*/,
    const winrt::Microsoft::ReactNative::JSValueObject &) {
  return xaml::Controls::Canvas();
}

void DebuggingOverlayViewManager::DispatchCommand(
    const XamlView &viewToUpdate,
    const std::string &commandId,
    winrt::Microsoft::ReactNative::JSValueArray &&commandArgs) {
  if (commandId == DebuggingOverlayCommands::Draw) {
    // The current spec has a Draw command -- but this will be replaced with some different commands in
    // https://github.com/facebook/react-native/pull/42119
    // There is little point in attempting to implement these commands until then.
    return;
  }
}

} // namespace Microsoft::ReactNative
