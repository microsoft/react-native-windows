/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import * as React from 'react';
import PopupNativeComponent from './PopupNativeComponent';
import type {ViewProps} from '../View/ViewPropTypes';
import {findNodeHandle} from '../../ReactNative/RendererProxy';
import StyleSheet from '../../StyleSheet/StyleSheet';

const warnOnce = require('../../Utilities/warnOnce').default;

const styles = StyleSheet.create({
  rctPopup: {
    position: 'absolute',
  },
});

type Props = $ReadOnly<{|
  ...ViewProps,

  isOpen?: boolean,

  /**
   * Configures the Popup with a transparent backdrop.
   */
  isLightDismissEnabled?: boolean,

  /**
   * Sets whether the Popup will automatically receive focus when opened.
   * Defaults to true. Always true when isLightDismissEnabled === true.
   */
  autoFocus?: boolean,

  horizontalOffset?: number,
  verticalOffset?: number,

  /**
   * Sets a React element to use as an anchor point. When set, the popup will be positioned relative to it.
   */
  target?: React.ReactNode,
  onDismiss?: () => void,
  testID?: string,
|}>;

type State = $ReadOnly<{|
  target?: number | null,
  targetRef?: React.ReactNode,
|}>;

const isFabric = global.nativeFabricUIManager;

/**
 * Renders a popup component.
 *
 * This is a controlled component that requires an `onDismiss` callback that
 * updates the `isOpen` prop in order for the component to reflect user actions.
 *
 * @keyword popup
 */
export class Popup extends React.Component<Props, State> {
  static getDerivedStateFromProps(nextProps: Props, prevState: State): State {
    // Check if we're given a new target property; we need to resolve it to a node handle before render
    if (prevState.targetRef !== nextProps.target) {
      // Map the 'target' property to a node tag to use in the native layer
      const newtarget = findNodeHandle(nextProps.target);
      return {
        target: newtarget,
        targetRef: nextProps.target,
      };
    }

    return prevState;
  }

  constructor(props: Props) {
    super(props);
    this.state = {target: undefined, targetRef: null};

    if (__DEV__ && isFabric) {
      warnOnce(
        'popup-new-arch-deprecated',
        '`Popup` is deprecated and not supported in the New Architecture. Use the new `Modal` component instead.',
      );
    }
  }

  render(): React.Node {
    const props = {...this.props};
    props.style = [styles.rctPopup, this.props.style];

    return <PopupNativeComponent {...props} target={this.state.target} />;
  }
}
