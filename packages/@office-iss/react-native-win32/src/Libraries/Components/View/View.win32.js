/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow strict-local
 */

import type {ViewProps} from './ViewPropTypes';

import ReactNativeFeatureFlags from '../../ReactNative/ReactNativeFeatureFlags';
import flattenStyle from '../../StyleSheet/flattenStyle';
import TextAncestor from '../../Text/TextAncestor';
import ViewNativeComponent from './ViewNativeComponent';
import * as React from 'react';
import invariant from 'invariant'; // [Windows]
// [Windows
import type {KeyEvent} from '../../Types/CoreEventTypes';
// Windows]

export type Props = ViewProps;

/**
 * The most fundamental component for building a UI, View is a container that
 * supports layout with flexbox, style, some touch handling, and accessibility
 * controls.
 *
 * @see https://reactnative.dev/docs/view
 */
const View: React.AbstractComponent<
  ViewProps,
  React.ElementRef<typeof ViewNativeComponent>,
> = React.forwardRef(
  (
    {
      accessibilityControls, // Win32
      accessibilityDescribedBy, // Win32
      accessibilityDescription, //  Win32
      accessibilityElementsHidden,
      accessibilityLabel,
      accessibilityLabelledBy,
      accessibilityLevel, // Win32
      accessibilityLiveRegion,
      accessibilityPositionInSet, // Win32
      accessibilitySetSize, // Win32
      accessibilityState,
      accessibilityValue,
      'aria-busy': ariaBusy,
      'aria-checked': ariaChecked,
      'aria-controls': ariaControls,
      'aria-describedby': ariaDescribedBy, // Win32
      'aria-description': ariaDescription, // Win32
      'aria-disabled': ariaDisabled,
      'aria-expanded': ariaExpanded,
      'aria-hidden': ariaHidden,
      'aria-label': ariaLabel,
      'aria-labelledby': ariaLabelledBy,
      'aria-level': ariaLevel, // Win32
      'aria-live': ariaLive,
      'aria-multiselectable': ariaMultiselectable, // Win32
      'aria-posinset': ariaPosinset, // Win32
      'aria-required': ariaRequired, // Win32
      'aria-selected': ariaSelected,
      'aria-setsize': ariaSetsize, // Win32
      'aria-valuemax': ariaValueMax,
      'aria-valuemin': ariaValueMin,
      'aria-valuenow': ariaValueNow,
      'aria-valuetext': ariaValueText,
      focusable,
      id,
      importantForAccessibility,
      nativeID,
      pointerEvents,
      tabIndex,
      ...otherProps
    }: ViewProps,
    forwardedRef,
  ) => {
    const _accessibilityLabelledBy =
      ariaLabelledBy?.split(/\s*,\s*/g) ?? accessibilityLabelledBy;

    let _accessibilityState;
    if (
      accessibilityState != null ||
      ariaBusy != null ||
      ariaChecked != null ||
      ariaDisabled != null ||
      ariaExpanded != null ||
      ariaMultiselectable != null ||
      ariaRequired != null ||
      ariaSelected != null
    ) {
      _accessibilityState = {
        busy: ariaBusy ?? accessibilityState?.busy,
        checked: ariaChecked ?? accessibilityState?.checked,
        disabled: ariaDisabled ?? accessibilityState?.disabled,
        expanded: ariaExpanded ?? accessibilityState?.expanded,
        multiselectable:
          ariaMultiselectable ?? accessibilityState?.multiselectable, // Win32
        required: ariaRequired ?? accessibilityState?.required, // Win32
        selected: ariaSelected ?? accessibilityState?.selected,
      };
    }
    let _accessibilityValue;
    if (
      accessibilityValue != null ||
      ariaValueMax != null ||
      ariaValueMin != null ||
      ariaValueNow != null ||
      ariaValueText != null
    ) {
      _accessibilityValue = {
        max: ariaValueMax ?? accessibilityValue?.max,
        min: ariaValueMin ?? accessibilityValue?.min,
        now: ariaValueNow ?? accessibilityValue?.now,
        text: ariaValueText ?? accessibilityValue?.text,
      };
    }

    // $FlowFixMe[underconstrained-implicit-instantiation]
    let style = flattenStyle(otherProps.style);

    const newPointerEvents = style?.pointerEvents || pointerEvents;
    const collapsableOverride =
      ReactNativeFeatureFlags.shouldForceUnflattenForElevation()
        ? {
            collapsable:
              style != null && style.elevation != null && style.elevation !== 0
                ? false
                : otherProps.collapsable,
          }
        : {};

    const _keyDown = (event: KeyEvent) => {
      if (otherProps.keyDownEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyDownEvents was already checked to not be undefined
        for (const el of otherProps.keyDownEvents) {
          if (event.nativeEvent.code == el.code && el.handledEventPhase == 3) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyDown && otherProps.onKeyDown(event);
    };

    const _keyUp = (event: KeyEvent) => {
      if (otherProps.keyUpEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyDownEvents was already checked to not be undefined
        for (const el of otherProps.keyUpEvents) {
          if (event.nativeEvent.code == el.code && el.handledEventPhase == 3) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyUp && otherProps.onKeyUp(event);
    };

    const _keyDownCapture = (event: KeyEvent) => {
      if (otherProps.keyDownEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyDownEvents was already checked to not be undefined
        for (const el of otherProps.keyDownEvents) {
          if (event.nativeEvent.code == el.code && el.handledEventPhase == 1) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyDownCapture && otherProps.onKeyDownCapture(event);
    };

    const _keyUpCapture = (event: KeyEvent) => {
      if (otherProps.keyUpEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyDownEvents was already checked to not be undefined
        for (const el of otherProps.keyUpEvents) {
          if (event.nativeEvent.code == el.code && el.handledEventPhase == 1) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyUpCapture && otherProps.onKeyUpCapture(event);
    };

    // [Windows
    // $FlowFixMe - children typing
    const childrenWithImportantForAccessibility = children => {
      return React.Children.map(children, child => {
        if (React.isValidElement(child)) {
          // $FlowFixMe[incompatible-use]
          if (child.props.children) {
            // $FlowFixMe[incompatible-type]
            return React.cloneElement(child, {
              accessible: false,
              children: childrenWithImportantForAccessibility(
                child.props.children,
              ),
            });
          } else {
            // $FlowFixMe[incompatible-type]
            return React.cloneElement(child, {accessible: false});
          }
        }
        return child;
      });
    };
    // Windows]

    return (
      // [Windows
      // In core this is a TextAncestor.Provider value={false} See
      // https://github.com/facebook/react-native/commit/66601e755fcad10698e61d20878d52194ad0e90c
      // But since Views are not currently supported in Text, we do not need the extra provider
      <TextAncestor.Consumer>
        {hasTextAncestor => {
          invariant(
            !hasTextAncestor,
            'Nesting of <View> within <Text> is not currently supported.',
          );
          return (
            <ViewNativeComponent
              {...otherProps}
              {...collapsableOverride}
              accessibilityControls={ariaControls ?? accessibilityControls} // Win32
              accessibilityDescribedBy={
                ariaDescribedBy ?? accessibilityDescribedBy
              } // Win32
              accessibilityDescription={
                ariaDescription ?? accessibilityDescription
              } // Win32
              accessibilityLiveRegion={
                ariaLive === 'off'
                  ? 'none'
                  : ariaLive ?? accessibilityLiveRegion
              }
              accessibilityLabel={ariaLabel ?? accessibilityLabel}
              accessibilityLevel={ariaLevel ?? accessibilityLevel} // Win32
              focusable={tabIndex !== undefined ? !tabIndex : focusable}
              accessibilityPositionInSet={
                ariaPosinset ?? accessibilityPositionInSet
              } // Win32
              accessibilitySetSize={ariaSetsize ?? accessibilitySetSize} // Win32
              accessibilityState={_accessibilityState}
              accessibilityElementsHidden={
                ariaHidden ?? accessibilityElementsHidden
              }
              accessibilityLabelledBy={_accessibilityLabelledBy}
              accessibilityValue={_accessibilityValue}
              importantForAccessibility={
                ariaHidden === true
                  ? 'no-hide-descendants'
                  : importantForAccessibility
              }
              nativeID={id ?? nativeID}
              style={style}
              pointerEvents={newPointerEvents}
              ref={forwardedRef}
              onKeyDown={_keyDown}
              onKeyDownCapture={_keyDownCapture}
              onKeyUp={_keyUp}
              onKeyUpCapture={_keyUpCapture}
              // [Windows
              accessible={
                importantForAccessibility === 'no-hide-descendants'
                  ? false
                  : otherProps.accessible
              }
              children={
                importantForAccessibility === 'no-hide-descendants'
                  ? childrenWithImportantForAccessibility(otherProps.children)
                  : otherProps.children
              }
              // Windows]
            />
          );
        }}
      </TextAncestor.Consumer>
      // Windows]
    );
  },
);

View.displayName = 'View';

module.exports = View;
