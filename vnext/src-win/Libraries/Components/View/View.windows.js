/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import type {HostInstance} from '../../../src/private/types/HostInstance';
import type {SafeAreaInsetsChangeEvent} from '../../Types/CoreEventTypes';
import type {ViewProps} from './ViewPropTypes';

import TextAncestorContext from '../../Text/TextAncestorContext';
import ViewNativeComponent from './ViewNativeComponent';
import * as React from 'react';
import {use} from 'react';

const warnOnRepeatedSafeAreaInsetsChanges: (
  onSafeAreaInsetsChange: (event: SafeAreaInsetsChangeEvent) => unknown,
) => (event: SafeAreaInsetsChangeEvent) => unknown = __DEV__
  ? require('../../../src/private/components/view/warnOnRepeatedSafeAreaInsetsChanges')
      .default
  : onSafeAreaInsetsChange => onSafeAreaInsetsChange;

export type ViewInstance = HostInstance;

/**
 * The most fundamental component for building a UI. `View` is a container that
 * supports layout with flexbox, style, some touch handling, and accessibility
 * controls. `View` maps directly to the native view equivalent on whatever
 * platform React Native is running on, whether that is a `UIView`, `<div>`,
 * `android.view`, etc. `View` is designed to be nested inside other views and
 * can have 0 to many children of any type.
 *
 * @see https://reactnative.dev/docs/view
 */
component View(ref?: React.RefSetter<ViewInstance>, ...props: ViewProps) {
  const hasTextAncestor = use(TextAncestorContext);

  const {
    accessibilityState,
    accessibilityValue,
    'aria-busy': ariaBusy,
    'aria-checked': ariaChecked,
    'aria-disabled': ariaDisabled,
    'aria-expanded': ariaExpanded,
    'aria-multiselectable': ariaMultiselectable, // Windows
    'aria-required': ariaRequired, // Windows
    'aria-description': ariaDescription, //Windows
    'aria-hidden': ariaHidden,
    'aria-label': ariaLabel,
    'aria-labelledby': ariaLabelledBy,
    'aria-level': ariaLevel,
    'aria-live': ariaLive,
    'aria-modal': ariaModal,
    'aria-posinset': ariaPosinset, // Windows
    'aria-readonly': ariaReadOnly, // Windows
    'aria-selected': ariaSelected,
    'aria-setsize': ariaSetsize, // Windows
    'aria-valuemax': ariaValueMax,
    'aria-valuemin': ariaValueMin,
    'aria-valuenow': ariaValueNow,
    'aria-valuetext': ariaValueText,
    id,
    tabIndex,
    ...otherProps
  } = props;

  const resolvedProps = otherProps as {...ViewProps};

  const parsedAriaLabelledBy = ariaLabelledBy?.split(/\s*,\s*/g);
  if (parsedAriaLabelledBy !== undefined) {
    resolvedProps.accessibilityLabelledBy = parsedAriaLabelledBy;
  }

  if (ariaLabel !== undefined) {
    resolvedProps.accessibilityLabel = ariaLabel;
  }

  if (ariaLive !== undefined) {
    resolvedProps.accessibilityLiveRegion =
      ariaLive === 'off' ? 'none' : ariaLive;
  }

  if (ariaModal !== undefined) {
    resolvedProps.accessibilityViewIsModal = ariaModal;
  }

  if (ariaHidden !== undefined) {
    resolvedProps.accessibilityElementsHidden = ariaHidden;
    if (ariaHidden === true) {
      resolvedProps.importantForAccessibility = 'no-hide-descendants';
    }
  }

  // Windows accessibility properties
  if (ariaLevel !== undefined) {
    resolvedProps.accessibilityLevel = ariaLevel;
  }

  if (ariaDescription !== undefined) {
    resolvedProps.accessibilityDescription = ariaDescription;
  }

  if (ariaPosinset !== undefined) {
    resolvedProps.accessibilityPosInSet = ariaPosinset;
  }

  if (ariaSetsize !== undefined) {
    resolvedProps.accessibilitySetSize = ariaSetsize;
  }

  if (id !== undefined) {
    resolvedProps.nativeID = id;
  }

  if (tabIndex !== undefined) {
    resolvedProps.focusable = !tabIndex;
  }

  if (
    accessibilityState != null ||
    ariaBusy != null ||
    ariaChecked != null ||
    ariaDisabled != null ||
    ariaExpanded != null ||
    ariaSelected != null ||
    ariaReadOnly != null || // Windows
    ariaMultiselectable != null || // Windows
    ariaRequired != null // Windows
  ) {
    resolvedProps.accessibilityState = {
      busy: ariaBusy ?? accessibilityState?.busy,
      checked: ariaChecked ?? accessibilityState?.checked,
      disabled: ariaDisabled ?? accessibilityState?.disabled,
      expanded: ariaExpanded ?? accessibilityState?.expanded,
      selected: ariaSelected ?? accessibilityState?.selected,
      readOnly: ariaReadOnly ?? accessibilityState?.readOnly, // Windows
      multiselectable:
        ariaMultiselectable ?? accessibilityState?.multiselectable, // Windows
      required: ariaRequired ?? accessibilityState?.required, // Windows
    };
  }

  if (
    accessibilityValue != null ||
    ariaValueMax != null ||
    ariaValueMin != null ||
    ariaValueNow != null ||
    ariaValueText != null
  ) {
    resolvedProps.accessibilityValue = {
      max: ariaValueMax ?? accessibilityValue?.max,
      min: ariaValueMin ?? accessibilityValue?.min,
      now: ariaValueNow ?? accessibilityValue?.now,
      text: ariaValueText ?? accessibilityValue?.text,
    };
  }
  // [Windows key event processing and accessible property
  if (otherProps.keyDownEvents || otherProps.onKeyDown) {
    const keydownLocal = otherProps.onKeyDown;
    resolvedProps.onKeyDown = event => {
      if (otherProps.keyDownEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe[incompatible-type] - keyDownEvents was already checked to not be undefined
        for (const el of otherProps.keyDownEvents) {
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
      keydownLocal && keydownLocal(event);
    };
  }

  if (otherProps.keyUpEvents || otherProps.onKeyUp) {
    const keyupLocal = otherProps.onKeyUp;
    resolvedProps.onKeyUp = event => {
      if (otherProps.keyUpEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe[incompatible-type] - keyUpEvents was already checked to not be undefined
        for (const el of otherProps.keyUpEvents) {
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
      keyupLocal && keyupLocal(event);
    };
  }

  if (otherProps.keyDownEvents || otherProps.onKeyDownCapture) {
    const keydownCaptureLocal = otherProps.onKeyDownCapture;
    resolvedProps.onKeyDownCapture = event => {
      if (otherProps.keyDownEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe[incompatible-type] - keyDownEvents was already checked to not be undefined
        for (const el of otherProps.keyDownEvents) {
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
      keydownCaptureLocal && keydownCaptureLocal(event);
    };
  }

  if (otherProps.keyUpEvents || otherProps.onKeyUpCapture) {
    const keyupCaptureLocal = otherProps.onKeyUpCapture;
    resolvedProps.onKeyUpCapture = event => {
      if (otherProps.keyUpEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe[incompatible-type] - keyUpEvents was already checked to not be undefined
        for (const el of otherProps.keyUpEvents) {
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
      keyupCaptureLocal && keyupCaptureLocal(event);
    };
  }

  // Windows accessible property
  const computedAccessible =
    resolvedProps.importantForAccessibility === 'no-hide-descendants'
      ? false
      : otherProps.accessible;
  if (computedAccessible !== undefined) {
    resolvedProps.accessible = computedAccessible;
  }

  if (__DEV__) {
    const onSafeAreaInsetsChange =
      resolvedProps.experimental_onSafeAreaInsetsChange;
    if (onSafeAreaInsetsChange != null) {
      resolvedProps.experimental_onSafeAreaInsetsChange =
        warnOnRepeatedSafeAreaInsetsChanges(onSafeAreaInsetsChange);
    }
  }

  const actualView =
    ref == null ? (
      <ViewNativeComponent {...resolvedProps} />
    ) : (
      <ViewNativeComponent {...resolvedProps} ref={ref} />
    );

  if (hasTextAncestor) {
    return (
      <TextAncestorContext value={false}>{actualView}</TextAncestorContext>
    );
  }
  return actualView;
}

View.displayName = 'View';

export default View;
