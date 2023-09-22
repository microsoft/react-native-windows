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

void HostPlatformViewEventEmitter::onMouseEnter(PointerEvent const &pointerEvent) const {
  dispatchEvent(
      "mouseEnter",
      std::make_shared<PointerEvent>(pointerEvent),
      EventPriority::AsynchronousBatched,
      RawEvent::Category::ContinuousStart);
}

void HostPlatformViewEventEmitter::onMouseLeave(PointerEvent const &pointerEvent) const {
  dispatchEvent(
      "mouseLeave",
      std::make_shared<PointerEvent>(pointerEvent),
      EventPriority::AsynchronousBatched,
      RawEvent::Category::ContinuousStart);
}

} // namespace facebook::react

// @generated SignedSource<<a6f36c63efa0edf80beb29b0187deb77>>
