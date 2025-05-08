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

  tooltip?: string, // Win32
}>;

const RCTTouchableNativeFeedback = requireNativeComponent<Props>(
  'RCTTouchableNativeFeedback',
);

class TouchableNativeFeedback extends React.Component<Props, {}> {
  render(): React.Node {
    return (
      <RCTTouchableNativeFeedback {...this.props}>
        {this.props.children}
      </RCTTouchableNativeFeedback>
    );
  }
}

export default TouchableNativeFeedback;
