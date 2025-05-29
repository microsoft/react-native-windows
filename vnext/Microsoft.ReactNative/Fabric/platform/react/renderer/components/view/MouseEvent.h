// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/PointerEvent.h>

namespace facebook::react {

struct MouseEvent : public PointerEvent {
  MouseEvent(PointerEvent &event) : PointerEvent(event){};

  // We override the type so that it is not recorded as a PointerType,
  // otherwise PointerEventsProcessor gets confused by the Windows specific MouseEvents
  EventPayloadType getType() const override {
    return static_cast<EventPayloadType>(-1);
  };
};

} // namespace facebook::react