// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "HostPlatformViewProps.h"

#include <react/renderer/components/view/conversions.h>
#include <react/renderer/core/graphicsConversions.h>
#include <react/renderer/core/propsConversions.h>
#include <react/utils/CoreFeatures.h>

namespace facebook::react {

HostPlatformViewProps::HostPlatformViewProps(
    const PropsParserContext &context,
    const HostPlatformViewProps &sourceProps,
    const RawProps &rawProps,
    bool shouldSetRawProps)
    : BaseViewProps(context, sourceProps, rawProps),
      windowsEvents(
          CoreFeatures::enablePropIteratorSetter ? sourceProps.windowsEvents
                                                 : convertRawProp(context, rawProps, sourceProps.windowsEvents, {})),
      enableFocusRing(
          CoreFeatures::enablePropIteratorSetter
              ? sourceProps.enableFocusRing
              : convertRawProp(context, rawProps, "enableFocusRing", sourceProps.enableFocusRing, true)),
      focusable(
          CoreFeatures::enablePropIteratorSetter
              ? sourceProps.focusable
              : convertRawProp(context, rawProps, "focusable", sourceProps.focusable, {})) {}

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

void HostPlatformViewProps::setProp(
    const PropsParserContext &context,
    RawPropsPropNameHash hash,
    const char *propName,
    RawValue const &value) {
  // All Props structs setProp methods must always, unconditionally,
  // call all super::setProp methods, since multiple structs may
  // reuse the same values.
  BaseViewProps::setProp(context, hash, propName, value);

  static auto defaults = HostPlatformViewProps{};

  switch (hash) {
    WINDOWS_VIEW_EVENT_CASE(Focus);
    WINDOWS_VIEW_EVENT_CASE(Blur);
    WINDOWS_VIEW_EVENT_CASE(KeyUp);
    WINDOWS_VIEW_EVENT_CASE(KeyDown);
    WINDOWS_VIEW_EVENT_CASE(MouseEnter);
    WINDOWS_VIEW_EVENT_CASE(MouseLeave);
    RAW_SET_PROP_SWITCH_CASE_BASIC(enableFocusRing);
    RAW_SET_PROP_SWITCH_CASE_BASIC(focusable);
    RAW_SET_PROP_SWITCH_CASE_BASIC(keyDownEvents);
    RAW_SET_PROP_SWITCH_CASE_BASIC(keyUpEvents);
  }
}

} // namespace facebook::react
