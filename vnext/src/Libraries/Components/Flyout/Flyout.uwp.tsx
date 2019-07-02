/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import * as React from 'react';
import {findNodeHandle, requireNativeComponent, StyleSheet} from 'react-native';
import {IFlyoutProps} from './FlyoutProps';

const styles = StyleSheet.create({
  rctFlyout: {
    position: 'absolute',
  },
});

export interface IFlyoutTargetState {
  target?: number | null;
  targetRef?: React.ReactNode;
}

const RCTFlyout = requireNativeComponent('RCTFlyout');

/**
 * Renders a flyout component.
 *
 * This is a controlled component that requires an `onDismiss` callback that
 * updates the `isOpen` prop in order for the component to reflect user actions.
 *
 * @keyword flyout
 */
export class Flyout extends React.Component<IFlyoutProps, IFlyoutTargetState> {
  public static getDerivedStateFromProps(
    nextProps: IFlyoutProps,
    prevState: IFlyoutTargetState,
  ): IFlyoutTargetState {
    // Check if we're given a new target property; we need to resolve it to a node handle before render
    if (prevState.targetRef !== nextProps.target) {
      // Map the 'target' property to a node tag to use in the native layer
      /* tslint:disable-next-line no-any */
      const newtarget: number | null = findNodeHandle(nextProps.target as
        | null
        | number
        | React.Component<IFlyoutProps, IFlyoutTargetState>
        | React.ComponentClass<Flyout>);

      return {
        target: newtarget,
        targetRef: nextProps.target,
      };
    }

    return prevState;
  }

  constructor(props: IFlyoutProps) {
    super(props);
    this.state = {target: undefined, targetRef: null};
  }

  public render(): JSX.Element {
    const props = {...this.props};

    return (
      <RCTFlyout
        {...props}
        target={this.state.target}
        style={[styles.rctFlyout, this.props.style]}
      />
    );
  }
}

export default Flyout;
