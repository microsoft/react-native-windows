/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow strict-local
 * @format
 */
'use strict';

import type {GestureResponderEvent} from '../../Types/CoreEventTypes';
import type {TouchableWithoutFeedbackProps} from './TouchableWithoutFeedback';

import Pressability, {
  type PressabilityConfig,
} from '../../Pressability/Pressability';
import {findHostInstance_DEPRECATED} from '../../ReactNative/RendererProxy';
import processColor from '../../StyleSheet/processColor';
import Platform from '../../Utilities/Platform';
import {Commands} from '../View/ViewNativeComponent';
import invariant from 'invariant';
import * as React from 'react';

type TVProps = {
  /**
   * *(Apple TV only)* TV preferred focus (see documentation for the View component).
   *
   * @platform ios
   */
  hasTVPreferredFocus?: ?boolean,

  /**
   * Designates the next view to receive focus when the user navigates down. See the Android documentation.
   *
   * @platform android
   */
  nextFocusDown?: ?number,

  /**
   * Designates the next view to receive focus when the user navigates forward. See the Android documentation.
   *
   * @platform android
   */
  nextFocusForward?: ?number,

  /**
   * Designates the next view to receive focus when the user navigates left. See the Android documentation.
   *
   * @platform android
   */
  nextFocusLeft?: ?number,

  /**
   * Designates the next view to receive focus when the user navigates right. See the Android documentation.
   *
   * @platform android
   */
  nextFocusRight?: ?number,

  /**
   * Designates the next view to receive focus when the user navigates up. See the Android documentation.
   *
   * @platform android
   */
  nextFocusUp?: ?number,
};

export type TouchableNativeFeedbackProps = $ReadOnly<{
  ...TouchableWithoutFeedbackProps,
  ...TVProps,
  /**
   * Determines the type of background drawable that's going to be used to display feedback.
   * It takes an object with type property and extra data depending on the type.
   * It's recommended to use one of the following static methods to generate that dictionary:
   *      1) TouchableNativeFeedback.SelectableBackground() - will create object that represents android theme's
   *         default background for selectable elements (?android:attr/selectableItemBackground)
   *      2) TouchableNativeFeedback.SelectableBackgroundBorderless() - will create object that represent android
   *         theme's default background for borderless selectable elements
   *         (?android:attr/selectableItemBackgroundBorderless). Available on android API level 21+
   *      3) TouchableNativeFeedback.Ripple(color, borderless) - will create object that represents ripple drawable
   *         with specified color (as a string). If property borderless evaluates to true the ripple will render
   *         outside of the view bounds (see native actionbar buttons as an example of that behavior). This background
   *         type is available on Android API level 21+
   */
  background?: ?(
    | $ReadOnly<{
        type: 'ThemeAttrAndroid',
        attribute:
          | 'selectableItemBackground'
          | 'selectableItemBackgroundBorderless',
        rippleRadius: ?number,
      }>
    | $ReadOnly<{
        type: 'RippleAndroid',
        color: ?number,
        borderless: boolean,
        rippleRadius: ?number,
      }>
  ),
  /**
   * Set to true to add the ripple effect to the foreground of the view, instead
   * of the background. This is useful if one of your child views has a
   * background of its own, or you're e.g. displaying images, and you don't want
   * the ripple to be covered by them.
   *
   * Check TouchableNativeFeedback.canUseNativeForeground() first, as this is
   * only available on Android 6.0 and above. If you try to use this on older
   * versions, this will fallback to background.
   */
  useForeground?: ?boolean,

  tooltip?: string, // Win32
}>;

type State = $ReadOnly<{
  pressability: Pressability,
}>;

/**
 * A wrapper for making views respond properly to touches (Android only).
 * On Android this component uses native state drawable to display touch feedback.
 * At the moment it only supports having a single View instance as a child node,
 * as it's implemented by replacing that View with another instance of RCTView node with some additional properties set.
 *
 * Background drawable of native feedback touchable can be customized with background property.
 *
 * @see https://reactnative.dev/docs/touchablenativefeedback#content
 */
class TouchableNativeFeedback extends React.Component<
  TouchableNativeFeedbackProps,
  State,
> {
  /**
   * Creates an object that represents android theme's default background for
   * selectable elements (?android:attr/selectableItemBackground).
   *
   * @param rippleRadius The radius of ripple effect
   */
  static SelectableBackground: (rippleRadius?: ?number) => $ReadOnly<{
    attribute: 'selectableItemBackground',
    type: 'ThemeAttrAndroid',
    rippleRadius: ?number,
  }> = (rippleRadius?: ?number) => ({
    type: 'ThemeAttrAndroid',
    attribute: 'selectableItemBackground',
    rippleRadius,
  });

  /**
   * Creates an object that represent android theme's default background for borderless
   * selectable elements (?android:attr/selectableItemBackgroundBorderless).
   * Available on android API level 21+.
   *
   * @param rippleRadius The radius of ripple effect
   */
  static SelectableBackgroundBorderless: (rippleRadius?: ?number) => $ReadOnly<{
    attribute: 'selectableItemBackgroundBorderless',
    type: 'ThemeAttrAndroid',
    rippleRadius: ?number,
  }> = (rippleRadius?: ?number) => ({
    type: 'ThemeAttrAndroid',
    attribute: 'selectableItemBackgroundBorderless',
    rippleRadius,
  });

  /**
   * Creates an object that represents ripple drawable with specified color (as a
   * string). If property `borderless` evaluates to true the ripple will
   * render outside of the view bounds (see native actionbar buttons as an
   * example of that behavior). This background type is available on Android
   * API level 21+.
   *
   * @param color The ripple color
   * @param borderless If the ripple can render outside it's bounds
   * @param rippleRadius The radius of ripple effect
   */
  static Ripple: (
    color: string,
    borderless: boolean,
    rippleRadius?: ?number,
  ) => $ReadOnly<{
    borderless: boolean,
    color: ?number,
    rippleRadius: ?number,
    type: 'RippleAndroid',
  }> = (color: string, borderless: boolean, rippleRadius?: ?number) => {
    const processedColor = processColor(color);
    invariant(
      processedColor == null || typeof processedColor === 'number',
      'Unexpected color given for Ripple color',
    );
    return {
      type: 'RippleAndroid',
      // $FlowFixMe[incompatible-type]
      color: processedColor,
      borderless,
      rippleRadius,
    };
  };

  /**
   * Whether `useForeground` is supported.
   */
  static canUseNativeForeground: () => boolean = () =>
    Platform.OS === 'android';

  state: State = {
    pressability: new Pressability(this._createPressabilityConfig()),
  };

  _createPressabilityConfig(): PressabilityConfig {
    const accessibilityStateDisabled =
      this.props['aria-disabled'] ?? this.props.accessibilityState?.disabled;
    return {
      cancelable: !this.props.rejectResponderTermination,
      disabled:
        this.props.disabled != null
          ? this.props.disabled
          : accessibilityStateDisabled,
      hitSlop: this.props.hitSlop,
      delayLongPress: this.props.delayLongPress,
      delayPressIn: this.props.delayPressIn,
      delayPressOut: this.props.delayPressOut,
      minPressDuration: 0,
      pressRectOffset: this.props.pressRetentionOffset,
      android_disableSound: this.props.touchSoundDisabled,
      onLongPress: this.props.onLongPress,
      onPress: this.props.onPress,
      onPressIn: event => {
        if (Platform.OS === 'android') {
          this._dispatchHotspotUpdate(event);
          this._dispatchPressedStateChange(true);
        }
        if (this.props.onPressIn != null) {
          this.props.onPressIn(event);
        }
      },
      onPressMove: event => {
        if (Platform.OS === 'android') {
          this._dispatchHotspotUpdate(event);
        }
      },
      onPressOut: event => {
        if (Platform.OS === 'android') {
          this._dispatchPressedStateChange(false);
        }
        if (this.props.onPressOut != null) {
          this.props.onPressOut(event);
        }
      },
    };
  }

  _dispatchPressedStateChange(pressed: boolean): void {
    if (Platform.OS === 'android') {
      const hostComponentRef = findHostInstance_DEPRECATED(this);
      if (hostComponentRef == null) {
        console.warn(
          'Touchable: Unable to find HostComponent instance. ' +
            'Has your Touchable component been unmounted?',
        );
      } else {
        Commands.setPressed(hostComponentRef, pressed);
      }
    }
  }

  _dispatchHotspotUpdate(event: GestureResponderEvent): void {
    if (Platform.OS === 'android') {
      const {locationX, locationY} = event.nativeEvent;
      const hostComponentRef = findHostInstance_DEPRECATED(this);
      if (hostComponentRef == null) {
        console.warn(
          'Touchable: Unable to find HostComponent instance. ' +
            'Has your Touchable component been unmounted?',
        );
      } else {
        Commands.hotspotUpdate(
          hostComponentRef,
          locationX ?? 0,
          locationY ?? 0,
        );
      }
    }
  }

  componentDidUpdate(
    prevProps: TouchableNativeFeedbackProps,
    prevState: State,
  ) {
    this.state.pressability.configure(this._createPressabilityConfig());
  }

  componentDidMount(): mixed {
    this.state.pressability.configure(this._createPressabilityConfig());
  }

  componentWillUnmount(): void {
    this.state.pressability.reset();
  }
}

TouchableNativeFeedback.displayName = 'TouchableNativeFeedback';

export default TouchableNativeFeedback;
