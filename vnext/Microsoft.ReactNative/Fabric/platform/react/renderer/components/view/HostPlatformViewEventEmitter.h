// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/BaseViewEventEmitter.h>
#include "KeyEvent.h"
#include "MouseEvent.h"

namespace facebook::react {

struct PressEvent {
  Tag target;
  Point pagePoint;
  Point offsetPoint;
  double timestamp;
  int identifier;
};

class HostPlatformViewEventEmitter : public BaseViewEventEmitter {
 public:
  using BaseViewEventEmitter::BaseViewEventEmitter;

#pragma mark - Keyboard Events

  void onKeyDown(KeyEvent const &keyEvent) const;
  void onKeyUp(KeyEvent const &keyEvent) const;

#pragma mark - Focus Events

  void onFocus() const;
  void onBlur() const;

#pragma mark - Mouse Events

  void onMouseEnter(MouseEvent const &pointerEvent) const;
  void onMouseLeave(MouseEvent const &pointerEvent) const;

#pragma mark - Touch Events

  virtual void onPressIn(PressEvent event) const;
};

} // namespace facebook::react
