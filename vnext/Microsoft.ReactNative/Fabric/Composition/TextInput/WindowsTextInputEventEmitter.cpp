// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "WindowsTextInputEventEmitter.h"

#include <react/renderer/core/graphicsConversions.h>

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

static jsi::Value textInputMetricsContentSizePayload(
    jsi::Runtime &runtime,
    const WindowsTextInputEventEmitter::OnContentSizeChange &event) {
  auto payload = jsi::Object(runtime);
  {
    auto contentSize = jsi::Object(runtime);
    contentSize.setProperty(runtime, "width", event.contentSize.width);
    contentSize.setProperty(runtime, "height", event.contentSize.height);
    payload.setProperty(runtime, "contentSize", contentSize);
  }
  return payload;
};

void WindowsTextInputEventEmitter::onContentSizeChange(OnContentSizeChange event) const {
  dispatchEvent("textInputContentSizeChange", [event = std::move(event)](jsi::Runtime &runtime) {
    return textInputMetricsContentSizePayload(runtime, event);
  });
}

void WindowsTextInputEventEmitter::onPressIn(GestureResponderEvent event) const {
  dispatchEvent("textInputPressIn", [event = std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    auto nativeEvent = jsi::Object(runtime);
    nativeEvent.setProperty(runtime, "target", static_cast<double>(event.target));
    nativeEvent.setProperty(runtime, "pageX", event.pagePoint.x);
    nativeEvent.setProperty(runtime, "pageY", event.pagePoint.y);
    nativeEvent.setProperty(runtime, "locationX", event.offsetPoint.x);
    nativeEvent.setProperty(runtime, "locationY", event.offsetPoint.y);
    nativeEvent.setProperty(runtime, "timestamp", event.timestamp);
    nativeEvent.setProperty(runtime, "identifier", static_cast<double>(event.identifier));
    payload.setProperty(runtime, "nativeEvent", nativeEvent);
    return payload;
  });
}

void WindowsTextInputEventEmitter::onEndEditing(OnEndEditing event) const {
  dispatchEvent("textInputEndEditing", [event = std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "eventCount", event.eventCount);
    payload.setProperty(runtime, "target", event.target);
    payload.setProperty(runtime, "text", event.text);
    return payload;
  });
}

void WindowsTextInputEventEmitter::onPressOut(GestureResponderEvent event) const {
  dispatchEvent("textInputPressOut", [event = std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    auto nativeEvent = jsi::Object(runtime);
    nativeEvent.setProperty(runtime, "target", static_cast<double>(event.target));
    nativeEvent.setProperty(runtime, "pageX", event.pagePoint.x);
    nativeEvent.setProperty(runtime, "pageY", event.pagePoint.y);
    nativeEvent.setProperty(runtime, "locationX", event.offsetPoint.x);
    nativeEvent.setProperty(runtime, "locationY", event.offsetPoint.y);
    nativeEvent.setProperty(runtime, "timestamp", event.timestamp);
    nativeEvent.setProperty(runtime, "identifier", static_cast<double>(event.identifier));
    payload.setProperty(runtime, "nativeEvent", nativeEvent);
    return payload;
  });
}

void WindowsTextInputEventEmitter::onScroll(facebook::react::Point &offset) const {
  dispatchEvent("textInputScroll", [offset = std::move(offset)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    {
      auto contentOffsetObj = jsi::Object(runtime);
      contentOffsetObj.setProperty(runtime, "x", offset.x);
      contentOffsetObj.setProperty(runtime, "y", offset.y);
      payload.setProperty(runtime, "contentOffset", contentOffsetObj);
    }
    return payload;
  });
}

} // namespace facebook::react
