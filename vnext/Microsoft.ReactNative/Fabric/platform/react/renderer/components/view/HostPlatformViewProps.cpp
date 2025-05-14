// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "HostPlatformViewProps.h"

#include <react/featureflags/ReactNativeFeatureFlags.h>
#include <react/renderer/components/view/conversions.h>
#include <react/renderer/core/graphicsConversions.h>
#include <react/renderer/core/propsConversions.h>

namespace facebook::react {

HostPlatformViewProps::HostPlatformViewProps(
    const PropsParserContext &context,
    const HostPlatformViewProps &sourceProps,
    const RawProps &rawProps,
    bool shouldSetRawProps)
    : BaseViewProps(context, sourceProps, rawProps),
      windowsEvents(
          ReactNativeFeatureFlags::enableCppPropsIteratorSetter()
              ? sourceProps.windowsEvents
              : convertRawProp(context, rawProps, sourceProps.windowsEvents, {})),
      accessibilityAnnotation(
          ReactNativeFeatureFlags::enableCppPropsIteratorSetter()
              ? sourceProps.accessibilityAnnotation
              : convertRawProp(context, rawProps, "accessibilityAnnotation", sourceProps.accessibilityAnnotation, {})),
      enableFocusRing(
          ReactNativeFeatureFlags::enableCppPropsIteratorSetter()
              ? sourceProps.enableFocusRing
              : convertRawProp(context, rawProps, "enableFocusRing", sourceProps.enableFocusRing, true)),
      focusable(
          ReactNativeFeatureFlags::enableCppPropsIteratorSetter()
              ? sourceProps.focusable
              : convertRawProp(context, rawProps, "focusable", sourceProps.focusable, {})),
      tooltip(
          ReactNativeFeatureFlags::enableCppPropsIteratorSetter()
              ? sourceProps.tooltip
              : convertRawProp(context, rawProps, "tooltip", sourceProps.tooltip, {})),
      accessibilityPosInSet(
          ReactNativeFeatureFlags::enableCppPropsIteratorSetter()
              ? sourceProps.accessibilityPosInSet
              : convertRawProp(context, rawProps, "accessibilityPosInSet", sourceProps.accessibilityPosInSet, 0)),
      accessibilitySetSize(
          ReactNativeFeatureFlags::enableCppPropsIteratorSetter()
              ? sourceProps.accessibilitySetSize
              : convertRawProp(context, rawProps, "accessibilitySetSize", sourceProps.accessibilitySetSize, 0)),
      accessibilityLevel(
          ReactNativeFeatureFlags::enableCppPropsIteratorSetter()
              ? sourceProps.accessibilityLevel
              : convertRawProp(context, rawProps, "accessibilityLevel", sourceProps.accessibilityLevel, 0)),
      accessibilityItemType(
          ReactNativeFeatureFlags::enableCppPropsIteratorSetter()
              ? sourceProps.accessibilityItemType
              : convertRawProp(context, rawProps, "accessibilityItemType", sourceProps.accessibilityItemType, {})),
      accessibilityAccessKey(
          ReactNativeFeatureFlags::enableCppPropsIteratorSetter()
              ? sourceProps.accessibilityAccessKey
              : convertRawProp(context, rawProps, "accessibilityAccessKey", sourceProps.accessibilityAccessKey, {})),
      accessibilityLiveRegion(
          ReactNativeFeatureFlags::enableCppPropsIteratorSetter() ? sourceProps.accessibilityLiveRegion
                                                                  : convertRawProp(
                                                                        context,
                                                                        rawProps,
                                                                        "accessibilityLiveRegion",
                                                                        sourceProps.accessibilityLiveRegion,
                                                                        "none")) {}

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
    RAW_SET_PROP_SWITCH_CASE_BASIC(accessibilityAnnotation);
    RAW_SET_PROP_SWITCH_CASE_BASIC(focusable);
    RAW_SET_PROP_SWITCH_CASE_BASIC(accessibilityPosInSet);
    RAW_SET_PROP_SWITCH_CASE_BASIC(accessibilitySetSize);
    RAW_SET_PROP_SWITCH_CASE_BASIC(accessibilityLevel);
    RAW_SET_PROP_SWITCH_CASE_BASIC(accessibilityItemType);
    RAW_SET_PROP_SWITCH_CASE_BASIC(accessibilityAccessKey);
    RAW_SET_PROP_SWITCH_CASE_BASIC(accessibilityLiveRegion);
    RAW_SET_PROP_SWITCH_CASE_BASIC(keyDownEvents);
    RAW_SET_PROP_SWITCH_CASE_BASIC(keyUpEvents);
    RAW_SET_PROP_SWITCH_CASE_BASIC(tooltip);
  }
}

} // namespace facebook::react
