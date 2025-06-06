// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "HostPlatformViewEventEmitter.h"

namespace facebook::react {

#pragma mark - Keyboard Events

static jsi::Value keyEventPayload(jsi::Runtime &runtime, KeyEvent const &event) {
  auto payload = jsi::Object(runtime);
  payload.setProperty(runtime, "key", jsi::String::createFromUtf8(runtime, event.key));
  payload.setProperty(runtime, "code", jsi::String::createFromUtf8(runtime, event.code));
  payload.setProperty(runtime, "ctrlKey", event.ctrlKey);
  payload.setProperty(runtime, "shiftKey", event.shiftKey);
  payload.setProperty(runtime, "altKey", event.altKey);
  payload.setProperty(runtime, "metaKey", event.metaKey);
  return payload;
};

void HostPlatformViewEventEmitter::onKeyDown(KeyEvent const &keyEvent) const {
  dispatchEvent("keyDown", [keyEvent](jsi::Runtime &runtime) { return keyEventPayload(runtime, keyEvent); });
}

void HostPlatformViewEventEmitter::onKeyUp(KeyEvent const &keyEvent) const {
  dispatchEvent("keyUp", [keyEvent](jsi::Runtime &runtime) { return keyEventPayload(runtime, keyEvent); });
}

#pragma mark - Focus Events

void HostPlatformViewEventEmitter::onFocus() const {
  dispatchEvent("focus");
}

void HostPlatformViewEventEmitter::onBlur() const {
  dispatchEvent("blur");
}

#pragma mark - Mouse Events

void HostPlatformViewEventEmitter::onMouseEnter(MouseEvent const &pointerEvent) const {
  dispatchEvent("mouseEnter", std::make_shared<MouseEvent>(pointerEvent), RawEvent::Category::ContinuousStart);
}

void HostPlatformViewEventEmitter::onMouseLeave(MouseEvent const &pointerEvent) const {
  dispatchEvent("mouseLeave", std::make_shared<MouseEvent>(pointerEvent), RawEvent::Category::ContinuousStart);
}

#pragma mark - Touch Events

void HostPlatformViewEventEmitter::onPressIn(GestureResponderEvent event) const {
  dispatchEvent("pressIn", [event](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    auto nativeEvent = jsi::Object(runtime);
    nativeEvent.setProperty(runtime, "target", static_cast<double>(event.target));
    nativeEvent.setProperty(runtime, "pageX", event.pagePoint.x);
    nativeEvent.setProperty(runtime, "pageY", event.pagePoint.y);
    nativeEvent.setProperty(runtime, "locationX", event.offsetPoint.x);
    nativeEvent.setProperty(runtime, "locationY", event.offsetPoint.y);
    nativeEvent.setProperty(runtime, "timestamp", event.timestamp);
    nativeEvent.setProperty(runtime, "identifier", event.identifier);
    payload.setProperty(runtime, "nativeEvent", nativeEvent);
    return payload;
  });
}

} // namespace facebook::react
