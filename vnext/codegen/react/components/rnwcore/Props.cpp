
/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <react/components/rnwcore/Props.h>
#include <react/components/image/conversions.h>
#include <react/core/propsConversions.h>

namespace facebook {
namespace react {

ModalHostViewProps::ModalHostViewProps(
    const ModalHostViewProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    animationType(convertRawProp(rawProps, "animationType", sourceProps.animationType, {ModalHostViewAnimationType::None})),
    presentationStyle(convertRawProp(rawProps, "presentationStyle", sourceProps.presentationStyle, {ModalHostViewPresentationStyle::FullScreen})),
    transparent(convertRawProp(rawProps, "transparent", sourceProps.transparent, {false})),
    statusBarTranslucent(convertRawProp(rawProps, "statusBarTranslucent", sourceProps.statusBarTranslucent, {false})),
    hardwareAccelerated(convertRawProp(rawProps, "hardwareAccelerated", sourceProps.hardwareAccelerated, {false})),
    visible(convertRawProp(rawProps, "visible", sourceProps.visible, {false})),
    animated(convertRawProp(rawProps, "animated", sourceProps.animated, {false})),
    supportedOrientations(convertRawProp(rawProps, "supportedOrientations", sourceProps.supportedOrientations, {static_cast<ModalHostViewSupportedOrientationsMask>(ModalHostViewSupportedOrientations::Portrait)})),
    identifier(convertRawProp(rawProps, "identifier", sourceProps.identifier, {0}))
      {}
ActivityIndicatorViewProps::ActivityIndicatorViewProps(
    const ActivityIndicatorViewProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    hidesWhenStopped(convertRawProp(rawProps, "hidesWhenStopped", sourceProps.hidesWhenStopped, {false})),
    animating(convertRawProp(rawProps, "animating", sourceProps.animating, {false})),
    color(convertRawProp(rawProps, "color", sourceProps.color, {})),
    size(convertRawProp(rawProps, "size", sourceProps.size, {ActivityIndicatorViewSize::Small}))
      {}
DatePickerProps::DatePickerProps(
    const DatePickerProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    date(convertRawProp(rawProps, "date", sourceProps.date, {0.0})),
    initialDate(convertRawProp(rawProps, "initialDate", sourceProps.initialDate, {0.0})),
    locale(convertRawProp(rawProps, "locale", sourceProps.locale, {})),
    maximumDate(convertRawProp(rawProps, "maximumDate", sourceProps.maximumDate, {0.0})),
    minimumDate(convertRawProp(rawProps, "minimumDate", sourceProps.minimumDate, {0.0})),
    minuteInterval(convertRawProp(rawProps, "minuteInterval", sourceProps.minuteInterval, {DatePickerMinuteInterval::MinuteInterval1})),
    mode(convertRawProp(rawProps, "mode", sourceProps.mode, {DatePickerMode::Date})),
    timeZoneOffsetInMinutes(convertRawProp(rawProps, "timeZoneOffsetInMinutes", sourceProps.timeZoneOffsetInMinutes, {0.0})),
    pickerStyle(convertRawProp(rawProps, "pickerStyle", sourceProps.pickerStyle, {DatePickerPickerStyle::Spinner}))
      {}
AndroidDrawerLayoutProps::AndroidDrawerLayoutProps(
    const AndroidDrawerLayoutProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    keyboardDismissMode(convertRawProp(rawProps, "keyboardDismissMode", sourceProps.keyboardDismissMode, {AndroidDrawerLayoutKeyboardDismissMode::None})),
    drawerBackgroundColor(convertRawProp(rawProps, "drawerBackgroundColor", sourceProps.drawerBackgroundColor, {})),
    drawerPosition(convertRawProp(rawProps, "drawerPosition", sourceProps.drawerPosition, {AndroidDrawerLayoutDrawerPosition::Left})),
    drawerWidth(convertRawProp(rawProps, "drawerWidth", sourceProps.drawerWidth, {})),
    drawerLockMode(convertRawProp(rawProps, "drawerLockMode", sourceProps.drawerLockMode, {AndroidDrawerLayoutDrawerLockMode::Unlocked})),
    statusBarBackgroundColor(convertRawProp(rawProps, "statusBarBackgroundColor", sourceProps.statusBarBackgroundColor, {}))
      {}
RCTMaskedViewProps::RCTMaskedViewProps(
    const RCTMaskedViewProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps)

    
      {}
AndroidProgressBarProps::AndroidProgressBarProps(
    const AndroidProgressBarProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    styleAttr(convertRawProp(rawProps, "styleAttr", sourceProps.styleAttr, {})),
    typeAttr(convertRawProp(rawProps, "typeAttr", sourceProps.typeAttr, {})),
    indeterminate(convertRawProp(rawProps, "indeterminate", sourceProps.indeterminate, {false})),
    progress(convertRawProp(rawProps, "progress", sourceProps.progress, {0.0})),
    animating(convertRawProp(rawProps, "animating", sourceProps.animating, {true})),
    color(convertRawProp(rawProps, "color", sourceProps.color, {})),
    testID(convertRawProp(rawProps, "testID", sourceProps.testID, {""}))
      {}
RCTProgressViewProps::RCTProgressViewProps(
    const RCTProgressViewProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    progressViewStyle(convertRawProp(rawProps, "progressViewStyle", sourceProps.progressViewStyle, {RCTProgressViewProgressViewStyle::Default})),
    progress(convertRawProp(rawProps, "progress", sourceProps.progress, {0.0})),
    progressTintColor(convertRawProp(rawProps, "progressTintColor", sourceProps.progressTintColor, {})),
    trackTintColor(convertRawProp(rawProps, "trackTintColor", sourceProps.trackTintColor, {})),
    progressImage(convertRawProp(rawProps, "progressImage", sourceProps.progressImage, {})),
    trackImage(convertRawProp(rawProps, "trackImage", sourceProps.trackImage, {}))
      {}
AndroidSwipeRefreshLayoutProps::AndroidSwipeRefreshLayoutProps(
    const AndroidSwipeRefreshLayoutProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    enabled(convertRawProp(rawProps, "enabled", sourceProps.enabled, {true})),
    colors(convertRawProp(rawProps, "colors", sourceProps.colors, {})),
    progressBackgroundColor(convertRawProp(rawProps, "progressBackgroundColor", sourceProps.progressBackgroundColor, {})),
    size(convertRawProp(rawProps, "size", sourceProps.size, {AndroidSwipeRefreshLayoutSize::Default})),
    progressViewOffset(convertRawProp(rawProps, "progressViewOffset", sourceProps.progressViewOffset, {0.0})),
    refreshing(convertRawProp(rawProps, "refreshing", sourceProps.refreshing, {false}))
      {}
PullToRefreshViewProps::PullToRefreshViewProps(
    const PullToRefreshViewProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    tintColor(convertRawProp(rawProps, "tintColor", sourceProps.tintColor, {})),
    titleColor(convertRawProp(rawProps, "titleColor", sourceProps.titleColor, {})),
    title(convertRawProp(rawProps, "title", sourceProps.title, {})),
    progressViewOffset(convertRawProp(rawProps, "progressViewOffset", sourceProps.progressViewOffset, {0.0})),
    refreshing(convertRawProp(rawProps, "refreshing", sourceProps.refreshing, {false}))
      {}
SafeAreaViewProps::SafeAreaViewProps(
    const SafeAreaViewProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    emulateUnlessSupported(convertRawProp(rawProps, "emulateUnlessSupported", sourceProps.emulateUnlessSupported, {false}))
      {}
AndroidHorizontalScrollContentViewProps::AndroidHorizontalScrollContentViewProps(
    const AndroidHorizontalScrollContentViewProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps)

    
      {}
RCTSegmentedControlProps::RCTSegmentedControlProps(
    const RCTSegmentedControlProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    values(convertRawProp(rawProps, "values", sourceProps.values, {})),
    selectedIndex(convertRawProp(rawProps, "selectedIndex", sourceProps.selectedIndex, {0})),
    enabled(convertRawProp(rawProps, "enabled", sourceProps.enabled, {true})),
    tintColor(convertRawProp(rawProps, "tintColor", sourceProps.tintColor, {})),
    textColor(convertRawProp(rawProps, "textColor", sourceProps.textColor, {})),
    backgroundColor(convertRawProp(rawProps, "backgroundColor", sourceProps.backgroundColor, {})),
    momentary(convertRawProp(rawProps, "momentary", sourceProps.momentary, {false}))
      {}
SliderProps::SliderProps(
    const SliderProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    disabled(convertRawProp(rawProps, "disabled", sourceProps.disabled, {false})),
    enabled(convertRawProp(rawProps, "enabled", sourceProps.enabled, {true})),
    maximumTrackImage(convertRawProp(rawProps, "maximumTrackImage", sourceProps.maximumTrackImage, {})),
    maximumTrackTintColor(convertRawProp(rawProps, "maximumTrackTintColor", sourceProps.maximumTrackTintColor, {})),
    maximumValue(convertRawProp(rawProps, "maximumValue", sourceProps.maximumValue, {1.0})),
    minimumTrackImage(convertRawProp(rawProps, "minimumTrackImage", sourceProps.minimumTrackImage, {})),
    minimumTrackTintColor(convertRawProp(rawProps, "minimumTrackTintColor", sourceProps.minimumTrackTintColor, {})),
    minimumValue(convertRawProp(rawProps, "minimumValue", sourceProps.minimumValue, {0.0})),
    step(convertRawProp(rawProps, "step", sourceProps.step, {0.0})),
    testID(convertRawProp(rawProps, "testID", sourceProps.testID, {""})),
    thumbImage(convertRawProp(rawProps, "thumbImage", sourceProps.thumbImage, {})),
    thumbTintColor(convertRawProp(rawProps, "thumbTintColor", sourceProps.thumbTintColor, {})),
    trackImage(convertRawProp(rawProps, "trackImage", sourceProps.trackImage, {})),
    value(convertRawProp(rawProps, "value", sourceProps.value, {0.0}))
      {}
AndroidSwitchProps::AndroidSwitchProps(
    const AndroidSwitchProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    disabled(convertRawProp(rawProps, "disabled", sourceProps.disabled, {false})),
    enabled(convertRawProp(rawProps, "enabled", sourceProps.enabled, {true})),
    thumbColor(convertRawProp(rawProps, "thumbColor", sourceProps.thumbColor, {})),
    trackColorForFalse(convertRawProp(rawProps, "trackColorForFalse", sourceProps.trackColorForFalse, {})),
    trackColorForTrue(convertRawProp(rawProps, "trackColorForTrue", sourceProps.trackColorForTrue, {})),
    value(convertRawProp(rawProps, "value", sourceProps.value, {false})),
    on(convertRawProp(rawProps, "on", sourceProps.on, {false})),
    thumbTintColor(convertRawProp(rawProps, "thumbTintColor", sourceProps.thumbTintColor, {})),
    trackTintColor(convertRawProp(rawProps, "trackTintColor", sourceProps.trackTintColor, {}))
      {}
SwitchProps::SwitchProps(
    const SwitchProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    disabled(convertRawProp(rawProps, "disabled", sourceProps.disabled, {false})),
    value(convertRawProp(rawProps, "value", sourceProps.value, {false})),
    tintColor(convertRawProp(rawProps, "tintColor", sourceProps.tintColor, {})),
    onTintColor(convertRawProp(rawProps, "onTintColor", sourceProps.onTintColor, {})),
    thumbTintColor(convertRawProp(rawProps, "thumbTintColor", sourceProps.thumbTintColor, {})),
    thumbColor(convertRawProp(rawProps, "thumbColor", sourceProps.thumbColor, {})),
    trackColorForFalse(convertRawProp(rawProps, "trackColorForFalse", sourceProps.trackColorForFalse, {})),
    trackColorForTrue(convertRawProp(rawProps, "trackColorForTrue", sourceProps.trackColorForTrue, {}))
      {}
InputAccessoryProps::InputAccessoryProps(
    const InputAccessoryProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    backgroundColor(convertRawProp(rawProps, "backgroundColor", sourceProps.backgroundColor, {}))
      {}
UnimplementedNativeViewProps::UnimplementedNativeViewProps(
    const UnimplementedNativeViewProps &sourceProps,
    const RawProps &rawProps): ViewProps(sourceProps, rawProps),

    name(convertRawProp(rawProps, "name", sourceProps.name, {""}))
      {}

} // namespace react
} // namespace facebook
