/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import type {ViewProps} from './ViewPropTypes';

import TextAncestor from '../../Text/TextAncestor';
import ViewNativeComponent from './ViewNativeComponent';
import * as React from 'react';
import invariant from 'invariant'; // [Windows]
// [Windows
import type {KeyEvent} from '../../Types/CoreEventTypes';
// Windows]

/**
 * The most fundamental component for building a UI, View is a container that
 * supports layout with flexbox, style, some touch handling, and accessibility
 * controls.
 *
 * @see https://reactnative.dev/docs/view
 */
const View: component(
  ref?: React.RefSetter<React.ElementRef<typeof ViewNativeComponent>>,
  ...props: ViewProps
) = React.forwardRef(
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
      tabIndex,
      ...otherProps
    }: ViewProps,
    forwardedRef,
  ) => {
    const _accessibilityLabelledBy =
      ariaLabelledBy?.split(/\s*,\s*/g) ?? accessibilityLabelledBy;

    const _accessibilityState =
      accessibilityState != null ||
      ariaBusy != null ||
      ariaChecked != null ||
      ariaDisabled != null ||
      ariaExpanded != null ||
      ariaMultiselectable != null ||
      ariaRequired != null ||
      ariaSelected != null
        ? {
            busy: ariaBusy ?? accessibilityState?.busy,
            checked: ariaChecked ?? accessibilityState?.checked,
            disabled: ariaDisabled ?? accessibilityState?.disabled,
            expanded: ariaExpanded ?? accessibilityState?.expanded,
            multiselectable:
              ariaMultiselectable ?? accessibilityState?.multiselectable, // Win32
            required: ariaRequired ?? accessibilityState?.required, // Win32
            selected: ariaSelected ?? accessibilityState?.selected,
          }
        : undefined;

    const _accessibilityValue =
      accessibilityValue != null ||
      ariaValueMax != null ||
      ariaValueMin != null ||
      ariaValueNow != null ||
      ariaValueText != null
        ? {
            max: ariaValueMax ?? accessibilityValue?.max,
            min: ariaValueMin ?? accessibilityValue?.min,
            now: ariaValueNow ?? accessibilityValue?.now,
            text: ariaValueText ?? accessibilityValue?.text,
          }
        : undefined;

    const _keyDown = (event: KeyEvent) => {
      const keyDownEvents = otherProps.keyDownEvents;
      if (keyDownEvents != null && event.isPropagationStopped() !== true) {
        for (const el of keyDownEvents) {
          if (
            event.nativeEvent.code === el.code &&
            event.nativeEvent.ctrlKey === Boolean(el.ctrlKey) &&
            event.nativeEvent.shiftKey === Boolean(el.shiftKey) &&
            event.nativeEvent.altKey === Boolean(el.altKey) &&
            event.nativeEvent.metaKey === Boolean(el.metaKey) &&
            el.handledEventPhase === 3
          ) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyDown && otherProps.onKeyDown(event);
    };

    const _keyUp = (event: KeyEvent) => {
      const keyUpEvents = otherProps.keyUpEvents;
      if (keyUpEvents != null && event.isPropagationStopped() !== true) {
        for (const el of keyUpEvents) {
          if (
            event.nativeEvent.code === el.code &&
            event.nativeEvent.ctrlKey === Boolean(el.ctrlKey) &&
            event.nativeEvent.shiftKey === Boolean(el.shiftKey) &&
            event.nativeEvent.altKey === Boolean(el.altKey) &&
            event.nativeEvent.metaKey === Boolean(el.metaKey) &&
            el.handledEventPhase === 3
          ) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyUp && otherProps.onKeyUp(event);
    };

    const _keyDownCapture = (event: KeyEvent) => {
      const keyDownEvents = otherProps.keyDownEvents;
      if (keyDownEvents != null && event.isPropagationStopped() !== true) {
        for (const el of keyDownEvents) {
          if (
            event.nativeEvent.code === el.code &&
            event.nativeEvent.ctrlKey === Boolean(el.ctrlKey) &&
            event.nativeEvent.shiftKey === Boolean(el.shiftKey) &&
            event.nativeEvent.altKey === Boolean(el.altKey) &&
            event.nativeEvent.metaKey === Boolean(el.metaKey) &&
            el.handledEventPhase === 1
          ) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyDownCapture && otherProps.onKeyDownCapture(event);
    };

    const _keyUpCapture = (event: KeyEvent) => {
      const keyUpEvents = otherProps.keyUpEvents;
      if (keyUpEvents != null && event.isPropagationStopped() !== true) {
        for (const el of keyUpEvents) {
          if (
            event.nativeEvent.code === el.code &&
            event.nativeEvent.ctrlKey === Boolean(el.ctrlKey) &&
            event.nativeEvent.shiftKey === Boolean(el.shiftKey) &&
            event.nativeEvent.altKey === Boolean(el.altKey) &&
            event.nativeEvent.metaKey === Boolean(el.metaKey) &&
            el.handledEventPhase === 1
          ) {
            event.stopPropagation();
          }
        }
      }
      otherProps.onKeyUpCapture && otherProps.onKeyUpCapture(event);
    };

    // [Windows
    const childrenWithImportantForAccessibility = (
      children: React.Node,
    ): React.Node => {
      return React.Children.map(children, child => {
        if (React.isValidElement(child)) {
          // $FlowFixMe[incompatible-use]
          // $FlowFixMe[prop-missing]
          if (child.props.children) {
            // $FlowFixMe[incompatible-call] - React.Children.map types child as mixed
            // $FlowFixMe[incompatible-type]
            return React.cloneElement(child, {
              accessible: false,
              children: childrenWithImportantForAccessibility(
                child.props.children,
              ),
            });
          } else {
            // $FlowFixMe[incompatible-call]
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

export default View;
