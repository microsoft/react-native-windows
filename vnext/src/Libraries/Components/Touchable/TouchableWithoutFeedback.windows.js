/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import Pressability, {
  type PressabilityConfig,
} from '../../Pressability/Pressability';
import {PressabilityDebugView} from '../../Pressability/PressabilityDebug';
import type {
  AccessibilityActionEvent,
  AccessibilityActionInfo,
  AccessibilityRole,
  AccessibilityState,
  AccessibilityValue,
} from '../../Components/View/ViewAccessibility';
import type {EdgeInsetsProp} from '../../StyleSheet/EdgeInsetsPropType';
import type {
  BlurEvent,
  FocusEvent,
  LayoutEvent,
  MouseEvent, // [Windows]
  PressEvent,
} from '../../Types/CoreEventTypes';
import View from '../../Components/View/View';
import * as React from 'react';

type Props = $ReadOnly<{|
  accessibilityActions?: ?$ReadOnlyArray<AccessibilityActionInfo>,
  accessibilityElementsHidden?: ?boolean,
  accessibilityHint?: ?Stringish,
  accessibilityIgnoresInvertColors?: ?boolean,
  accessibilityLabel?: ?Stringish,
  accessibilityLiveRegion?: ?('none' | 'polite' | 'assertive'),
  accessibilityRole?: ?AccessibilityRole,
  accessibilityState?: ?AccessibilityState,
  accessibilityValue?: ?AccessibilityValue,
  accessibilityViewIsModal?: ?boolean,
  accessible?: ?boolean,
  children?: ?React.Node,
  delayLongPress?: ?number,
  delayPressIn?: ?number,
  delayPressOut?: ?number,
  disabled?: ?boolean,
  focusable?: ?boolean,
  hitSlop?: ?EdgeInsetsProp,
  importantForAccessibility?: ?('auto' | 'yes' | 'no' | 'no-hide-descendants'),
  nativeID?: ?string,
  onAccessibilityAction?: ?(event: AccessibilityActionEvent) => mixed,
  onBlur?: ?(event: BlurEvent) => mixed,
  onFocus?: ?(event: FocusEvent) => mixed,
  onLayout?: ?(event: LayoutEvent) => mixed,
  onLongPress?: ?(event: PressEvent) => mixed,
  onPress?: ?(event: PressEvent) => mixed,
  onPressIn?: ?(event: PressEvent) => mixed,
  onPressOut?: ?(event: PressEvent) => mixed,
  pressRetentionOffset?: ?EdgeInsetsProp,
  rejectResponderTermination?: ?boolean,
  testID?: ?string,
  touchSoundDisabled?: ?boolean,
  accessibilityPosInSet?: ?number, // [Windows]
  accessibilitySetSize?: ?number, // [Windows]
  onAccessibilityTap?: ?() => void, // [Windows]
  onMouseEnter?: ?(event: MouseEvent) => void, // [Windows]
  onMouseLeave?: ?(event: MouseEvent) => void, // [Windows]
  tabIndex?: ?number, // [Windows]
  tooltip?: ?Stringish, // [Windows]
|}>;

type State = $ReadOnly<{|
  pressability: Pressability,
|}>;

const PASSTHROUGH_PROPS = [
  'accessibilityActions',
  'accessibilityElementsHidden',
  'accessibilityHint',
  'accessibilityIgnoresInvertColors',
  'accessibilityLabel',
  'accessibilityLiveRegion',
  'accessibilityRole',
  'accessibilityValue',
  'accessibilityViewIsModal',
  'hitSlop',
  'importantForAccessibility',
  'nativeID',
  'onAccessibilityAction',
  'onBlur',
  'onFocus',
  'onLayout',
  'testID',
  'accessibilityPosInSet', // [Windows]
  'accessibilitySetSize', // [Windows]
  'onAccessibilityTap', // [Windows]
  'onMouseEnter', // [Windows]
  'onMouseLeave', // [Windows]
  'tabIndex', // [Windows]
  'tooltip', // [Windows]
];

class TouchableWithoutFeedback extends React.Component<Props, State> {
  state: State = {
    pressability: new Pressability(createPressabilityConfig(this.props)),
  };

  render(): React.Node {
    const element = React.Children.only(this.props.children);
    const children = [element.props.children];
    if (__DEV__) {
      if (element.type === View) {
        children.push(
          <PressabilityDebugView color="red" hitSlop={this.props.hitSlop} />,
        );
      }
    }

    // BACKWARD-COMPATIBILITY: Focus and blur events were never supported before
    // adopting `Pressability`, so preserve that behavior.
    const {onBlur, onFocus, ...eventHandlersWithoutBlurAndFocus} =
      this.state.pressability.getEventHandlers();

    const elementProps: {[string]: mixed, ...} = {
      ...eventHandlersWithoutBlurAndFocus,
      accessible: this.props.accessible !== false,
      accessibilityState:
        this.props.disabled != null
          ? {
              ...this.props.accessibilityState,
              disabled: this.props.disabled,
            }
          : this.props.accessibilityState,
      focusable:
        this.props.focusable !== false && this.props.onPress !== undefined,
    };
    for (const prop of PASSTHROUGH_PROPS) {
      if (this.props[prop] !== undefined) {
        elementProps[prop] = this.props[prop];
      }
    }

    return React.cloneElement(element, elementProps, ...children);
  }

  componentDidUpdate(): void {
    this.state.pressability.configure(createPressabilityConfig(this.props));
  }

  componentWillUnmount(): void {
    this.state.pressability.reset();
  }
}

function createPressabilityConfig(props: Props): PressabilityConfig {
  return {
    cancelable: !props.rejectResponderTermination,
    disabled:
      props.disabled !== null
        ? props.disabled
        : props.accessibilityState?.disabled,
    hitSlop: props.hitSlop,
    delayLongPress: props.delayLongPress,
    delayPressIn: props.delayPressIn,
    delayPressOut: props.delayPressOut,
    minPressDuration: 0,
    pressRectOffset: props.pressRetentionOffset,
    android_disableSound: props.touchSoundDisabled,
    onBlur: props.onBlur,
    onFocus: props.onFocus,
    onLongPress: props.onLongPress,
    onPress: props.onPress,
    onPressIn: props.onPressIn,
    onPressOut: props.onPressOut,
    onMouseEnter: props.onMouseEnter, // [Windows]
    onMouseLeave: props.onMouseLeave, // [Windows]
  };
}

TouchableWithoutFeedback.displayName = 'TouchableWithoutFeedback';

module.exports = TouchableWithoutFeedback;
