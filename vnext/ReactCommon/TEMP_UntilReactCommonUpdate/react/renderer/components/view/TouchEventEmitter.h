/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/components/view/PointerEvent.h>
#include <react/renderer/components/view/TouchEvent.h>
#include <react/renderer/core/EventEmitter.h>
#include <react/renderer/core/LayoutMetrics.h>
#include <react/renderer/core/ReactPrimitives.h>
#include <react/renderer/debug/DebugStringConvertible.h>

namespace facebook {
namespace react {

class TouchEventEmitter;

using SharedTouchEventEmitter = std::shared_ptr<TouchEventEmitter const>;

class TouchEventEmitter : public EventEmitter {
 public:
  using EventEmitter::EventEmitter;

  void onTouchStart(TouchEvent const &event) const;
  void onTouchMove(TouchEvent const &event) const;
  void onTouchEnd(TouchEvent const &event) const;
  void onTouchCancel(TouchEvent const &event) const;

  void onPointerCancel(PointerEvent const &event) const;
  void onPointerDown(PointerEvent const &event) const;
  void onPointerMove(PointerEvent const &event) const;
  void onPointerUp(PointerEvent const &event) const;
  void onPointerEnter(PointerEvent const &event) const;
  void onPointerLeave(PointerEvent const &event) const;
  void onPointerOver(PointerEvent const &event) const;
  void onPointerOut(PointerEvent const &event) const;

  void onMouseEnter(PointerEvent const &event) const; // [Windows]
  void onMouseLeave(PointerEvent const &event) const; // [Windows]

 private:
  void dispatchTouchEvent(
      std::string type,
      TouchEvent const &event,
      EventPriority priority,
      RawEvent::Category category) const;
  void dispatchPointerEvent(
      std::string type,
      PointerEvent const &event,
      EventPriority priority,
      RawEvent::Category category) const;
};

} // namespace react
} // namespace facebook
