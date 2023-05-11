/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * #7149 should be strict local
 * @flow
 * @format
 */

import type {PressEvent} from '../Types/CoreEventTypes';

import * as PressabilityDebug from '../Pressability/PressabilityDebug';
import usePressability from '../Pressability/usePressability';
import flattenStyle from '../StyleSheet/flattenStyle';
import processColor from '../StyleSheet/processColor';
import StyleSheet from '../StyleSheet/StyleSheet';
import {getAccessibilityRoleFromRole} from '../Utilities/AcessibilityMapping';
import Platform from '../Utilities/Platform';
import TextAncestor from './TextAncestor';
import {NativeText, NativeVirtualText} from './TextNativeComponent';
import {type TextProps} from './TextProps';
import * as React from 'react';
import {useContext, useMemo, useState} from 'react';

const View = require('../Components/View/View');
import {type TextStyleProp, type ViewStyleProp} from '../StyleSheet/StyleSheet'; // [Windows]

/**
 * Text is the fundamental component for displaying text.
 *
 * @see https://reactnative.dev/docs/text
 */
const Text: React.AbstractComponent<
  TextProps,
  React.ElementRef<typeof NativeText | typeof NativeVirtualText>,
> = React.forwardRef((props: TextProps, forwardedRef) => {
  const {
    accessible,
    accessibilityLabel,
    accessibilityRole,
    allowFontScaling,
    'aria-busy': ariaBusy,
    'aria-checked': ariaChecked,
    'aria-disabled': ariaDisabled,
    'aria-expanded': ariaExpanded,
    'aria-label': ariaLabel,
    'aria-selected': ariaSelected,
    ellipsizeMode,
    id,
    nativeID,
    onLongPress,
    onPress,
    onPressIn,
    onPressOut,
    onResponderGrant,
    onResponderMove,
    onResponderRelease,
    onResponderTerminate,
    onResponderTerminationRequest,
    onStartShouldSetResponder,
    pressRetentionOffset,
    role,
    suppressHighlighting,
    ...restProps
  } = props;

  const [isHighlighted, setHighlighted] = useState(false);

  const _accessibilityState = {
    busy: ariaBusy ?? props.accessibilityState?.busy,
    checked: ariaChecked ?? props.accessibilityState?.checked,
    disabled: ariaDisabled ?? props.accessibilityState?.disabled,
    expanded: ariaExpanded ?? props.accessibilityState?.expanded,
    selected: ariaSelected ?? props.accessibilityState?.selected,
  };

  const _disabled =
    restProps.disabled != null
      ? restProps.disabled
      : _accessibilityState?.disabled;

  const nativeTextAccessibilityState =
    _disabled !== _accessibilityState?.disabled
      ? {..._accessibilityState, disabled: _disabled}
      : _accessibilityState;

  const isPressable =
    (onPress != null ||
      onLongPress != null ||
      onStartShouldSetResponder != null) &&
    _disabled !== true;

  const initialized = useLazyInitialization(isPressable);
  const config = useMemo(
    () =>
      initialized
        ? {
            disabled: !isPressable,
            pressRectOffset: pressRetentionOffset,
            onLongPress,
            onPress,
            onPressIn(event: PressEvent) {
              setHighlighted(!suppressHighlighting);
              onPressIn?.(event);
            },
            onPressOut(event: PressEvent) {
              setHighlighted(false);
              onPressOut?.(event);
            },
            onResponderTerminationRequest_DEPRECATED:
              onResponderTerminationRequest,
            onStartShouldSetResponder_DEPRECATED: onStartShouldSetResponder,
          }
        : null,
    [
      initialized,
      isPressable,
      pressRetentionOffset,
      onLongPress,
      onPress,
      onPressIn,
      onPressOut,
      onResponderTerminationRequest,
      onStartShouldSetResponder,
      suppressHighlighting,
    ],
  );

  const eventHandlers = usePressability(config);
  const eventHandlersForText = useMemo(
    () =>
      eventHandlers == null
        ? null
        : {
            onResponderGrant(event: PressEvent) {
              eventHandlers.onResponderGrant(event);
              if (onResponderGrant != null) {
                onResponderGrant(event);
              }
            },
            onResponderMove(event: PressEvent) {
              eventHandlers.onResponderMove(event);
              if (onResponderMove != null) {
                onResponderMove(event);
              }
            },
            onResponderRelease(event: PressEvent) {
              eventHandlers.onResponderRelease(event);
              if (onResponderRelease != null) {
                onResponderRelease(event);
              }
            },
            onResponderTerminate(event: PressEvent) {
              eventHandlers.onResponderTerminate(event);
              if (onResponderTerminate != null) {
                onResponderTerminate(event);
              }
            },
            onClick: eventHandlers.onClick,
            onResponderTerminationRequest:
              eventHandlers.onResponderTerminationRequest,
            onStartShouldSetResponder: eventHandlers.onStartShouldSetResponder,
          },
    [
      eventHandlers,
      onResponderGrant,
      onResponderMove,
      onResponderRelease,
      onResponderTerminate,
    ],
  );

  // TODO: Move this processing to the view configuration.
  const selectionColor =
    restProps.selectionColor == null
      ? null
      : processColor(restProps.selectionColor);

  let style = flattenStyle(restProps.style);

  let _selectable = restProps.selectable;
  if (style?.userSelect != null) {
    _selectable = userSelectToSelectableMap[style.userSelect];
  }

  if (style?.verticalAlign != null) {
    style = StyleSheet.compose(style, {
      textAlignVertical:
        verticalAlignToTextAlignVerticalMap[style.verticalAlign],
    });
  }

  if (__DEV__) {
    if (PressabilityDebug.isEnabled() && onPress != null) {
      style = StyleSheet.compose(restProps.style, {
        color: 'magenta',
      });
    }
  }

  let numberOfLines = restProps.numberOfLines;
  if (numberOfLines != null && !(numberOfLines >= 0)) {
    console.error(
      `'numberOfLines' in <Text> must be a non-negative number, received: ${numberOfLines}. The value will be set to 0.`,
    );
    numberOfLines = 0;
  }

  const hasTextAncestor = useContext(TextAncestor);

  const _accessible = Platform.select({
    ios: accessible !== false,
    windows: accessible !== false,
    default: accessible,
  });

  let flattenedStyle = flattenStyle(style);

  if (typeof flattenedStyle?.fontWeight === 'number') {
    flattenedStyle.fontWeight = flattenedStyle?.fontWeight.toString();
  }

  const _hasOnPressOrOnLongPress =
    props.onPress != null || props.onLongPress != null;

  if (hasTextAncestor) {
    return (
      <NativeVirtualText
        {...restProps}
        accessibilityState={_accessibilityState}
        {...eventHandlersForText}
        accessibilityLabel={ariaLabel ?? accessibilityLabel}
        accessibilityRole={
          role ? getAccessibilityRoleFromRole(role) : accessibilityRole
        }
        isHighlighted={isHighlighted}
        isPressable={isPressable}
        selectable={_selectable}
        nativeID={id ?? nativeID}
        numberOfLines={numberOfLines}
        selectionColor={selectionColor}
        style={flattenedStyle}
        ref={forwardedRef}
      />
    );
  } else {
    let styleProps: ViewStyleProp = (restProps.style: any);
    if (
      styleProps &&
      styleProps.borderColor &&
      (styleProps.borderWidth ||
        styleProps.borderBottomWidth ||
        styleProps.borderEndWidth ||
        styleProps.borderLeftWidth ||
        styleProps.borderRightWidth ||
        styleProps.borderStartWidth ||
        styleProps.borderTopWidth)
    ) {
      let textStyleProps = Array.isArray(styleProps)
        ? StyleSheet.flatten(styleProps)
        : styleProps;
      let {
        margin,
        marginBottom,
        marginEnd,
        marginHorizontal,
        marginLeft,
        marginRight,
        marginStart,
        marginTop,
        marginVertical,
        padding,
        paddingBottom,
        paddingEnd,
        paddingHorizontal,
        paddingLeft,
        paddingRight,
        paddingStart,
        paddingTop,
        paddingVertical,
        ...rest
      } = textStyleProps != null ? textStyleProps : {};

      let {style, ...textPropsLessStyle} = props;
      return (
        <View style={styleProps}>
          <TextAncestor.Provider value={true}>
            <NativeText
              {...textPropsLessStyle}
              {...eventHandlersForText}
              disabled={_disabled}
              selectable={_selectable}
              accessible={
                accessible == null && Platform.OS === 'android'
                  ? _hasOnPressOrOnLongPress
                  : _accessible
              }
              accessibilityLabel={ariaLabel ?? accessibilityLabel}
              accessibilityState={nativeTextAccessibilityState}
              accessibilityRole={
                role ? getAccessibilityRoleFromRole(role) : accessibilityRole
              }
              allowFontScaling={allowFontScaling !== false}
              ellipsizeMode={ellipsizeMode ?? 'tail'}
              isHighlighted={isHighlighted}
              nativeID={id ?? nativeID}
              numberOfLines={numberOfLines}
              selectionColor={selectionColor}
              style={((rest: any): TextStyleProp)}
              ref={forwardedRef}
            />
          </TextAncestor.Provider>
        </View>
      );
    } else {
      return (
        <TextAncestor.Provider value={true}>
          <NativeText
            {...restProps}
            {...eventHandlersForText}
            disabled={_disabled}
            selectable={_selectable}
            accessible={
              accessible == null && Platform.OS === 'android'
                ? _hasOnPressOrOnLongPress
                : _accessible
            }
            accessibilityLabel={ariaLabel ?? accessibilityLabel}
            accessibilityState={nativeTextAccessibilityState}
            accessibilityRole={
              role ? getAccessibilityRoleFromRole(role) : accessibilityRole
            }
            allowFontScaling={allowFontScaling !== false}
            ellipsizeMode={ellipsizeMode ?? 'tail'}
            isHighlighted={isHighlighted}
            nativeID={id ?? nativeID}
            numberOfLines={numberOfLines}
            selectionColor={selectionColor}
            style={flattenedStyle}
            ref={forwardedRef}
          />
        </TextAncestor.Provider>
      );
    }
  }
});

Text.displayName = 'Text';

/**
 * Switch to `deprecated-react-native-prop-types` for compatibility with future
 * releases. This is deprecated and will be removed in the future.
 */
Text.propTypes = require('deprecated-react-native-prop-types').TextPropTypes;

/**
 * Returns false until the first time `newValue` is true, after which this will
 * always return true. This is necessary to lazily initialize `Pressability` so
 * we do not eagerly create one for every pressable `Text` component.
 */
function useLazyInitialization(newValue: boolean): boolean {
  const [oldValue, setValue] = useState(newValue);
  if (!oldValue && newValue) {
    setValue(newValue);
  }
  return oldValue;
}

const userSelectToSelectableMap = {
  auto: true,
  text: true,
  none: false,
  contain: true,
  all: true,
};

const verticalAlignToTextAlignVerticalMap = {
  auto: 'auto',
  top: 'top',
  bottom: 'bottom',
  middle: 'center',
};

module.exports = Text;
