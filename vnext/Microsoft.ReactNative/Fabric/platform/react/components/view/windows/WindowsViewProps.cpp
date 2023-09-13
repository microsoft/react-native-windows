/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "WindowsViewProps.h"

#include <react/renderer/components/view/conversions.h>
#include <react/renderer/core/graphicsConversions.h>
#include <react/renderer/core/propsConversions.h>
#include <react/utils/CoreFeatures.h>

namespace facebook::react {

WindowsViewProps::WindowsViewProps(
    const PropsParserContext &context,
    const WindowsViewProps &sourceProps,
    const RawProps &rawProps,
    bool shouldSetRawProps)
    : BaseViewProps(context, sourceProps, rawProps, shouldSetRawProps),
      windowsEvents(sourceProps.windowsEvents),
      enableFocusRing(sourceProps.enableFocusRing),
      elevation(
          CoreFeatures::enablePropIteratorSetter
              ? sourceProps.elevation
              : convertRawProp(context, rawProps, "elevation", sourceProps.elevation, {})),
      focusable(
          CoreFeatures::enablePropIteratorSetter
              ? sourceProps.focusable
              : convertRawProp(context, rawProps, "focusable", sourceProps.focusable, {})) {}
// [Windows
/*nativeBackground(
    CoreFeatures::enablePropIteratorSetter
        ? sourceProps.nativeBackground
        : convertRawProp(
              context,
              rawProps,
              "nativeBackgroundAndroid",
              sourceProps.nativeBackground,
              {})),
nativeForeground(
    CoreFeatures::enablePropIteratorSetter
        ? sourceProps.nativeForeground
        : convertRawProp(
              context,
              rawProps,
              "nativeForegroundAndroid",
              sourceProps.nativeForeground,
              {})),
hasTVPreferredFocus(
    CoreFeatures::enablePropIteratorSetter
        ? sourceProps.hasTVPreferredFocus
        : convertRawProp(
              context,
              rawProps,
              "hasTVPreferredFocus",
              sourceProps.hasTVPreferredFocus,
              {})),
needsOffscreenAlphaCompositing(
    CoreFeatures::enablePropIteratorSetter
        ? sourceProps.needsOffscreenAlphaCompositing
        : convertRawProp(
              context,
              rawProps,
              "needsOffscreenAlphaCompositing",
              sourceProps.needsOffscreenAlphaCompositing,
              {})),
renderToHardwareTextureAndroid(
    CoreFeatures::enablePropIteratorSetter
        ? sourceProps.renderToHardwareTextureAndroid
        : convertRawProp(
              context,
              rawProps,
              "renderToHardwareTextureAndroid",
              sourceProps.renderToHardwareTextureAndroid,
              {})) {}*/
// Windows]

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
  // All Props structs setProp methods must always, unconditionally,
  // call all super::setProp methods, since multiple structs may
  // reuse the same values.
  BaseViewProps::setProp(context, hash, propName, value);

  static auto defaults = WindowsViewProps{};

  switch (hash) {
    WINDOWS_VIEW_EVENT_CASE(Focus);
    WINDOWS_VIEW_EVENT_CASE(Blur);
    WINDOWS_VIEW_EVENT_CASE(KeyUp);
    WINDOWS_VIEW_EVENT_CASE(KeyDown);
    RAW_SET_PROP_SWITCH_CASE_BASIC(focusable);
    RAW_SET_PROP_SWITCH_CASE_BASIC(enableFocusRing);
    RAW_SET_PROP_SWITCH_CASE_BASIC(elevation);
    // [Windows
    // RAW_SET_PROP_SWITCH_CASE(nativeBackground, "nativeBackgroundAndroid");
    // RAW_SET_PROP_SWITCH_CASE(nativeForeground, "nativeForegroundAndroid");
    // RAW_SET_PROP_SWITCH_CASE_BASIC(hasTVPreferredFocus);
    // RAW_SET_PROP_SWITCH_CASE_BASIC(needsOffscreenAlphaCompositing);
    // RAW_SET_PROP_SWITCH_CASE_BASIC(renderToHardwareTextureAndroid);
    // Windows]
  }
}

#ifdef ANDROID
bool WindowsViewProps::getProbablyMoreHorizontalThanVertical_DEPRECATED() const {
  return yogaStyle.flexDirection() == YGFlexDirectionRow;
}
#endif

#if RN_DEBUG_STRING_CONVERTIBLE
SharedDebugStringConvertibleList WindowsViewProps::getDebugProps() const {
  return BaseViewProps::getDebugProps();
}
#endif

/*static*/ bool WindowsViewProps::requiresFormsStackingContext(const WindowsViewProps &props) noexcept {
  return props.windowsEvents.bits.any();
}

/*static*/ bool WindowsViewProps::requiresFormsView(const WindowsViewProps &props) noexcept {
  return props.focusable;
}

} // namespace facebook::react
