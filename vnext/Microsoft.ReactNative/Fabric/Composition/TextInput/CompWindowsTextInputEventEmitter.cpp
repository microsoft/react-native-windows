// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompWindowsTextInputEventEmitter.h"

namespace facebook::react {

void CompWindowsTextInputEventEmitter::onChange(OnChange event) const {
  dispatchEvent("change", [event = std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "eventCount", event.eventCount);
    payload.setProperty(runtime, "target", event.target);
    payload.setProperty(runtime, "text", event.text);
    return payload;
  });
}

void CompWindowsTextInputEventEmitter::onSelectionChange(const OnSelectionChange &event) const {
  dispatchEvent("textInputSelectionChange", [event = std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    auto selection = jsi::Object(runtime);
    selection.setProperty(runtime, "start", event.selection.start);
    selection.setProperty(runtime, "end", event.selection.end);
    payload.setProperty(runtime, "selection", selection);
    return payload;
  });
}

} // namespace facebook::react
