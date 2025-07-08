/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import type {ViewProps} from 'react-native';

import FlyoutNativeComponent from './FlyoutNativeComponent';
import * as React from 'react';
import {findNodeHandle, StyleSheet} from 'react-native';

const warnOnce = require('../../Utilities/warnOnce').default;

type Placement =
  | 'top'
  | 'bottom'
  | 'left'
  | 'right'
  | 'full'
  | 'top-edge-aligned-left'
  | 'top-edge-aligned-right'
  | 'bottom-edge-aligned-left'
  | 'bottom-edge-aligned-right'
  | 'left-edge-aligned-top'
  | 'right-edge-aligned-top'
  | 'left-edge-aligned-bottom'
  | 'right-edge-aligned-bottom';

type ShowMode =
  | 'auto'
  | 'standard'
  | 'transient'
  | 'transient-with-dismiss-on-pointer-move-away';

type Props = $ReadOnly<{
  ...ViewProps,

  // Props
  horizontalOffset?: Double,
  isLightDismissEnabled?: boolean,
  autoFocus?: boolean,
  shouldConstrainToRootBounds?: boolean,

  /**
   * Specifies whether the area outside the flyout is darkened
   */
  isOverlayEnabled?: boolean,

  isOpen?: boolean,
  onDismiss?: (isOpen: boolean) => void,
  placement?: Placement,
  showMode?: ShowMode,
  target?: React.ReactNode,
  verticalOffset?: number,

  // Events
}>;

const styles = StyleSheet.create({
  rctFlyout: {
    position: 'absolute',
  },
});

type State = $ReadOnly<{
  target?: number | null,
  targetRef?: React.ReactNode,
}>;

const isFabric = global.nativeFabricUIManager;

/**
 * Renders a flyout component.
 *
 * This is a controlled component that requires an `onDismiss` callback that
 * updates the `isOpen` prop in order for the component to reflect user actions.
 *
 * @keyword flyout
 */
export class Flyout extends React.Component<Props, State> {
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

  state = {target: undefined, targetRef: null};

  constructor(props: Props) {
    super(props);

    if (__DEV__ && isFabric) {
      warnOnce(
        'flyout-new-arch-deprecated',
        '`Flyout` is deprecated and not supported in the New Architecture. Use the new `Modal` component instead.',
      );
    }
  }

  render(): React.Node {
    const props = {...this.props};

    return (
      <FlyoutNativeComponent
        {...props}
        target={this.state.target}
        style={[styles.rctFlyout, this.props.style]}
      />
    );
  }
}
