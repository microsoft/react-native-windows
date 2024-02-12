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
import type {TextProps} from './TextProps';

import * as PressabilityDebug from '../Pressability/PressabilityDebug';
import usePressability from '../Pressability/usePressability';
import flattenStyle from '../StyleSheet/flattenStyle';
import processColor from '../StyleSheet/processColor';
import Platform from '../Utilities/Platform';
import TextAncestor from './TextAncestor';
import {NativeText, NativeVirtualText} from './TextNativeComponent';
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
    accessibilityLevel, // Windows
    accessibilityPosInSet, // Windows
    accessibilitySetSize, // Windows
    accessibilityState,
    allowFontScaling,
    'aria-busy': ariaBusy,
    'aria-checked': ariaChecked,
    'aria-disabled': ariaDisabled,
    'aria-expanded': ariaExpanded,
    'aria-label': ariaLabel,
    'aria-level': ariaLevel, // Windows
    'aria-posinset': ariaPosinset, // Windows
    'aria-setsize': ariaSetsize, // Windows
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
    suppressHighlighting,
    ...restProps
  } = props;

  const [isHighlighted, setHighlighted] = useState(false);

  let _accessibilityState;
  if (
    accessibilityState != null ||
    ariaBusy != null ||
    ariaChecked != null ||
    ariaDisabled != null ||
    ariaExpanded != null ||
    ariaSelected != null
  ) {
    _accessibilityState = {
      busy: ariaBusy ?? accessibilityState?.busy,
      checked: ariaChecked ?? accessibilityState?.checked,
      disabled: ariaDisabled ?? accessibilityState?.disabled,
      expanded: ariaExpanded ?? accessibilityState?.expanded,
      selected: ariaSelected ?? accessibilityState?.selected,
    };
  }

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
              // Updating isHighlighted causes unnecessary re-renders for platforms that don't use it
              // in the best case, and cause issues with text selection in the worst case. Forcing
              // the isHighlighted prop to false on all platforms except iOS.
              setHighlighted(
                (suppressHighlighting == null || !suppressHighlighting) &&
                  Platform.OS === 'ios',
              );
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

  let style = restProps.style;

  if (__DEV__) {
    if (PressabilityDebug.isEnabled() && onPress != null) {
      style = [restProps.style, {color: 'magenta'}];
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

  // $FlowFixMe[underconstrained-implicit-instantiation]
  style = flattenStyle(style);

  if (typeof style?.fontWeight === 'number') {
    // $FlowFixMe[prop-missing]
    // $FlowFixMe[cannot-write]
    style.fontWeight = style?.fontWeight.toString();
  }

  let _selectable = restProps.selectable;
  if (style?.userSelect != null) {
    // $FlowFixMe[invalid-computed-prop]
    _selectable = userSelectToSelectableMap[style.userSelect];
    // $FlowFixMe[prop-missing]
    // $FlowFixMe[cannot-write]
    delete style.userSelect;
  }

  if (style?.verticalAlign != null) {
    // $FlowFixMe[prop-missing]
    // $FlowFixMe[cannot-write]
    style.textAlignVertical =
      // $FlowFixMe[invalid-computed-prop]
      verticalAlignToTextAlignVerticalMap[style.verticalAlign];
    // $FlowFixMe[prop-missing]
    // $FlowFixMe[cannot-write]
    delete style.verticalAlign;
  }

  const _hasOnPressOrOnLongPress =
    props.onPress != null || props.onLongPress != null;

  if (hasTextAncestor) {
    return (
      <NativeVirtualText
        {...restProps}
        {...eventHandlersForText}
        accessibilityLabel={ariaLabel ?? accessibilityLabel}
        accessibilityLevel={ariaLevel ?? accessibilityLevel} // Windows
        accessibilityPosInSet={ariaPosinset ?? accessibilityPosInSet} // Windows
        accessibilitySetSize={ariaSetsize ?? accessibilitySetSize} // Windows
        accessibilityState={_accessibilityState}
        allowFontScaling={allowFontScaling !== false}
        disabled={_disabled}
        ellipsizeMode={ellipsizeMode ?? 'tail'}
        isHighlighted={isHighlighted}
        isPressable={isPressable}
        nativeID={id ?? nativeID}
        numberOfLines={numberOfLines}
        ref={forwardedRef}
        selectable={_selectable}
        selectionColor={selectionColor}
        style={style}
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
        ? // $FlowFixMe[underconstrained-implicit-instantiation]
          flattenStyle(styleProps)
        : styleProps;
      let {
        // $FlowFixMe[prop-missing]
        margin,
        // $FlowFixMe[prop-missing]
        marginBottom,
        // $FlowFixMe[prop-missing]
        marginEnd,
        // $FlowFixMe[prop-missing]
        marginHorizontal,
        // $FlowFixMe[prop-missing]
        marginLeft,
        // $FlowFixMe[prop-missing]
        marginRight,
        // $FlowFixMe[prop-missing]
        marginStart,
        // $FlowFixMe[prop-missing]
        marginTop,
        // $FlowFixMe[prop-missing]
        marginVertical,
        // $FlowFixMe[prop-missing]
        padding,
        // $FlowFixMe[prop-missing]
        paddingBottom,
        // $FlowFixMe[prop-missing]
        paddingEnd,
        // $FlowFixMe[prop-missing]
        paddingHorizontal,
        // $FlowFixMe[prop-missing]
        paddingLeft,
        // $FlowFixMe[prop-missing]
        paddingRight,
        // $FlowFixMe[prop-missing]
        paddingStart,
        // $FlowFixMe[prop-missing]
        paddingTop,
        // $FlowFixMe[prop-missing]
        paddingVertical,
        // $FlowFixMe[not-an-object]
        ...rest
      } = textStyleProps != null ? textStyleProps : {};

      let {style, ...textPropsLessStyle} = props;
      return (
        <View style={styleProps}>
          <TextAncestor.Provider value={true}>
            <NativeText
              {...textPropsLessStyle}
              {...eventHandlersForText}
              accessibilityLabel={ariaLabel ?? accessibilityLabel}
              accessibilityLevel={ariaLevel ?? accessibilityLevel} // Windows
              accessibilityPosInSet={ariaPosinset ?? accessibilityPosInSet} // Windows
              accessibilitySetSize={ariaSetsize ?? accessibilitySetSize} // Windows
              accessibilityState={nativeTextAccessibilityState}
              accessible={
                accessible == null && Platform.OS === 'android'
                  ? _hasOnPressOrOnLongPress
                  : _accessible
              }
              allowFontScaling={allowFontScaling !== false}
              disabled={_disabled}
              ellipsizeMode={ellipsizeMode ?? 'tail'}
              isHighlighted={isHighlighted}
              nativeID={id ?? nativeID}
              numberOfLines={numberOfLines}
              ref={forwardedRef}
              selectable={_selectable}
              selectionColor={selectionColor}
              style={((rest: any): TextStyleProp)}
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
            accessibilityLabel={ariaLabel ?? accessibilityLabel}
            accessibilityLevel={ariaLevel ?? accessibilityLevel} // Windows
            accessibilityPosInSet={ariaPosinset ?? accessibilityPosInSet} // Windows
            accessibilitySetSize={ariaSetsize ?? accessibilitySetSize} // Windows
            accessibilityState={nativeTextAccessibilityState}
            accessible={
              accessible == null && Platform.OS === 'android'
                ? _hasOnPressOrOnLongPress
                : _accessible
            }
            allowFontScaling={allowFontScaling !== false}
            disabled={_disabled}
            ellipsizeMode={ellipsizeMode ?? 'tail'}
            isHighlighted={isHighlighted}
            nativeID={id ?? nativeID}
            numberOfLines={numberOfLines}
            ref={forwardedRef}
            selectable={_selectable}
            selectionColor={selectionColor}
            style={style}
          />
        </TextAncestor.Provider>
      );
    }
  }
});

Text.displayName = 'Text';

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
