// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/BaseViewEventEmitter.h>
#include "KeyEvent.h"

namespace facebook::react {

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

  void onMouseEnter(PointerEvent const &pointerEvent) const;
  void onMouseLeave(PointerEvent const &pointerEvent) const;
};

} // namespace facebook::react
