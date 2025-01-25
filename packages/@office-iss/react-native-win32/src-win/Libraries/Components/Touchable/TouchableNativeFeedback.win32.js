/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow strict-local
 * @format
 */

'use strict';

import * as React from 'react';
import requireNativeComponent from '../../ReactNative/requireNativeComponent';
import typeof TouchableWithoutFeedback from './TouchableWithoutFeedback';

type Props = $ReadOnly<{
  ...React.ElementConfig<TouchableWithoutFeedback>,

  /**
   * Determines the type of background drawable that's going to be used to
   * display feedback. It takes an object with `type` property and extra data
   * depending on the `type`. It's recommended to use one of the static
   * methods to generate that dictionary.
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
   * TV preferred focus (see documentation for the View component).
   */
  hasTVPreferredFocus?: ?boolean,

  /**
   * TV next focus down (see documentation for the View component).
   */
  nextFocusDown?: ?number,

  /**
   * TV next focus forward (see documentation for the View component).
   */
  nextFocusForward?: ?number,

  /**
   * TV next focus left (see documentation for the View component).
   */
  nextFocusLeft?: ?number,

  /**
   * TV next focus right (see documentation for the View component).
   */
  nextFocusRight?: ?number,

  /**
   * TV next focus up (see documentation for the View component).
   */
  nextFocusUp?: ?number,

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
<<<<<<< Upstream
}>;

type State = $ReadOnly<{
  pressability: Pressability,
}>;

class TouchableNativeFeedback extends React.Component<Props, State> {
  /**
   * Creates a value for the `background` prop that uses the Android theme's
   * default background for selectable elements.
   */
  static SelectableBackground: (rippleRadius: ?number) => $ReadOnly<{
    attribute: 'selectableItemBackground',
    type: 'ThemeAttrAndroid',
    rippleRadius: ?number,
  }> = (rippleRadius: ?number) => ({
    type: 'ThemeAttrAndroid',
    attribute: 'selectableItemBackground',
    rippleRadius,
  });

  /**
   * Creates a value for the `background` prop that uses the Android theme's
   * default background for borderless selectable elements. Requires API 21+.
   */
  static SelectableBackgroundBorderless: (rippleRadius: ?number) => $ReadOnly<{
    attribute: 'selectableItemBackgroundBorderless',
    type: 'ThemeAttrAndroid',
    rippleRadius: ?number,
  }> = (rippleRadius: ?number) => ({
    type: 'ThemeAttrAndroid',
    attribute: 'selectableItemBackgroundBorderless',
    rippleRadius,
  });

  /**
   * Creates a value for the `background` prop that uses the Android ripple with
   * the supplied color. If `borderless` is true, the ripple will render outside
   * of the view bounds. Requires API 21+.
   */
  static Ripple: (
    color: string,
    borderless: boolean,
    rippleRadius: ?number,
  ) => $ReadOnly<{
    borderless: boolean,
    color: ?number,
    rippleRadius: ?number,
    type: 'RippleAndroid',
  }> = (color: string, borderless: boolean, rippleRadius: ?number) => {
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

  _dispatchHotspotUpdate(event: PressEvent): void {
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
=======

  tooltip?: string, // Win32
|}>;

const RCTTouchableNativeFeedback = requireNativeComponent<Props>(
  'RCTTouchableNativeFeedback',
);
>>>>>>> Override

class TouchableNativeFeedback extends React.Component<Props, {}> {
  render(): React.Node {
    return (
      <RCTTouchableNativeFeedback {...this.props}>
        {this.props.children}
      </RCTTouchableNativeFeedback>
    );
  }
}

module.exports = TouchableNativeFeedback;
