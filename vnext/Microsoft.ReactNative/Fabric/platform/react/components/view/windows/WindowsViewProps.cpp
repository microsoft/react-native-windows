/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "WindowsViewProps.h"

#include <react/renderer/core/CoreFeatures.h>
#include <react/renderer/core/propsConversions.h>

namespace facebook::react {

WindowsViewProps::WindowsViewProps(
    const PropsParserContext &context,
    const WindowsViewProps &sourceProps,
    const RawProps &rawProps,
    bool shouldSetRawProps)
    : windowsEvents(sourceProps.windowsEvents),
      focusable(sourceProps.focusable),
      enableFocusRing(sourceProps.enableFocusRing) {}

#define WINDOWS_VIEW_EVENT_CASE(eventType)                    \
  case CONSTEXPR_RAW_PROPS_KEY_HASH("on" #eventType): {       \
    const auto offset = WindowsViewEvents::Offset::eventType; \
    WindowsViewEvents defaultViewEvents{};                    \
    bool res = defaultViewEvents[offset];                     \
    if (value.hasValue()) {                                   \
      fromRawValue(context, value, res);                      \
    }                                                         \
    windowsEvents[offset] = res;                              \
    return;                                                   \
  }

void WindowsViewProps::setProp(
    const PropsParserContext &context,
    RawPropsPropNameHash hash,
    const char *propName,
    RawValue const &value) {
  static auto defaults = WindowsViewProps{};

  switch (hash) {
    WINDOWS_VIEW_EVENT_CASE(Focus);
    WINDOWS_VIEW_EVENT_CASE(Blur);
    WINDOWS_VIEW_EVENT_CASE(KeyUp);
    WINDOWS_VIEW_EVENT_CASE(KeyDown);
    RAW_SET_PROP_SWITCH_CASE_BASIC(focusable);
    RAW_SET_PROP_SWITCH_CASE_BASIC(enableFocusRing);
  }
}

/*static*/ bool WindowsViewProps::requiresFormsStackingContext(const WindowsViewProps &props) noexcept {
  return props.windowsEvents.bits.any();
}

/*static*/ bool WindowsViewProps::requiresFormsView(const WindowsViewProps &props) noexcept {
  return props.focusable;
}

} // namespace facebook::react
