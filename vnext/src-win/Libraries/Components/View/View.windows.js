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

import TextAncestorContext from '../../Text/TextAncestorContext';
import ViewNativeComponent from './ViewNativeComponent';
import * as React from 'react';
import {use} from 'react';
import invariant from 'invariant'; // [Windows]
// [Windows
import type {KeyEvent} from '../../Types/CoreEventTypes';
// Windows]

// [Windows
// $FlowFixMe - children typing
const childrenWithImportantForAccessibility = children => {
  if (children == null) {
    return children;
  }
  const updatedChildren = React.Children.map(children, child => {
    if (React.isValidElement(child)) {
      // $FlowFixMe[incompatible-use]
      if (child.props.children) {
        // $FlowFixMe[incompatible-call]
        // $FlowFixMe[incompatible-type]
        return React.cloneElement(child, {
          accessible: false,
          children: childrenWithImportantForAccessibility(child.props.children),
        });
      } else {
        // $FlowFixMe[incompatible-call]
        // $FlowFixMe[incompatible-type]
        return React.cloneElement(child, {accessible: false});
      }
    }
    return child;
  });
  if (updatedChildren.length === 1) {
    return updatedChildren[0];
  } else {
    return updatedChildren;
  }
};
// Windows]

/**
 * The most fundamental component for building a UI, View is a container that
 * supports layout with flexbox, style, some touch handling, and accessibility
 * controls.
 *
 * @see https://reactnative.dev/docs/view
 */
component View(
  ref?: React.RefSetter<React.ElementRef<typeof ViewNativeComponent>>,
  ...props: ViewProps
) {
  // eslint-disable-next-line react-hooks/rules-of-hooks
  const hasTextAncestor = use(TextAncestorContext);

  let actualView;
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

  // Since we destructured props, we can now treat it as mutable
  const processedProps = otherProps as {...ViewProps};

  const parsedAriaLabelledBy = ariaLabelledBy?.split(/\s*,\s*/g);
  if (parsedAriaLabelledBy !== undefined) {
    processedProps.accessibilityLabelledBy = parsedAriaLabelledBy;
  }

  if (ariaLabel !== undefined) {
    processedProps.accessibilityLabel = ariaLabel;
  }

  if (ariaLive !== undefined) {
    processedProps.accessibilityLiveRegion =
      ariaLive === 'off' ? 'none' : ariaLive;
  }

  if (ariaHidden !== undefined) {
    processedProps.accessibilityElementsHidden = ariaHidden;
    if (ariaHidden === true) {
      processedProps.importantForAccessibility = 'no-hide-descendants';
    }
  }

  // Windows accessibility properties
  if (ariaLevel !== undefined) {
    processedProps.accessibilityLevel = ariaLevel;
  }

  if (ariaDescription !== undefined) {
    processedProps.accessibilityDescription = ariaDescription;
  }

  if (ariaPosinset !== undefined) {
    processedProps.accessibilityPosInSet = ariaPosinset;
  }

  if (ariaSetsize !== undefined) {
    processedProps.accessibilitySetSize = ariaSetsize;
  }

  if (id !== undefined) {
    processedProps.nativeID = id;
  }

  if (tabIndex !== undefined) {
    processedProps.focusable = !tabIndex;
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
    processedProps.accessibilityState = {
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
    processedProps.accessibilityValue = {
      max: ariaValueMax ?? accessibilityValue?.max,
      min: ariaValueMin ?? accessibilityValue?.min,
      now: ariaValueNow ?? accessibilityValue?.now,
      text: ariaValueText ?? accessibilityValue?.text,
    };
  }

  // [Windows key event processing and accessible property
  if (otherProps.keyDownEvents || otherProps.onKeyDown) {
    const keydownLocal = otherProps.onKeyDown;
    processedProps.onKeyDown = event => {
      if (otherProps.keyDownEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyDownEvents was already checked to not be undefined
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
    processedProps.onKeyUp = event => {
      if (otherProps.keyUpEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyUpEvents was already checked to not be undefined
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
    processedProps.onKeyDownCapture = event => {
      if (otherProps.keyDownEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyDownEvents was already checked to not be undefined
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
    processedProps.onKeyUpCapture = event => {
      if (otherProps.keyUpEvents && event.isPropagationStopped() !== true) {
        // $FlowFixMe - keyUpEvents was already checked to not be undefined
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
    processedProps.importantForAccessibility === 'no-hide-descendants'
      ? false
      : otherProps.accessible;
  if (computedAccessible !== undefined) {
    processedProps.accessible = computedAccessible;
  }

  actualView =
    ref == null ? (
      <ViewNativeComponent {...processedProps} />
    ) : (
      <ViewNativeComponent {...processedProps} ref={ref} />
    );

  if (hasTextAncestor) {
    return (
      <TextAncestorContext value={false}>{actualView}</TextAncestorContext>
    );
  }
  return actualView;
}

// eslint-disable-next-line no-unreachable
View.displayName = 'View';

export default View;
