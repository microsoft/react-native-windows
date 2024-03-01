// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsTextInputEventEmitter.h"

namespace facebook::react {

void WindowsTextInputEventEmitter::onChange(OnChange event) const {
  dispatchEvent("change", [event = std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "eventCount", event.eventCount);
    payload.setProperty(runtime, "target", event.target);
    payload.setProperty(runtime, "text", event.text);
    return payload;
  });
}

void WindowsTextInputEventEmitter::onSelectionChange(const OnSelectionChange &event) const {
  dispatchEvent("textInputSelectionChange", [event = std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    auto selection = jsi::Object(runtime);
    selection.setProperty(runtime, "start", event.selection.start);
    selection.setProperty(runtime, "end", event.selection.end);
    payload.setProperty(runtime, "selection", selection);
    return payload;
  });
}

void WindowsTextInputEventEmitter::onSubmitEditing(OnSubmitEditing event) const {
  dispatchEvent("textInputSubmitEditing", [event = std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "eventCount", event.eventCount);
    payload.setProperty(runtime, "target", event.target);
    payload.setProperty(runtime, "text", event.text);
    return payload;
  });
}

void WindowsTextInputEventEmitter::onKeyPress(OnKeyPress event) const {
  dispatchEvent("textInputKeyPress", [event = std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "key", event.key);
    return payload;
  });
}

} // namespace facebook::react
