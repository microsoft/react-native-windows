// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import * as React from 'react';
import { findNodeHandle, requireNativeComponent, StyleSheet } from 'react-native';
import { IMenuFlyoutProps } from './MenuFlyoutProps';

const styles = StyleSheet.create({
  rctMenuFlyout: {
    position: 'absolute',
  },
});

export interface IMenuFlyoutTargetState {
  target?: number | null;
  targetRef?: React.ReactNode;
}

const RCTMenuFlyout = requireNativeComponent('RCTMenuFlyout');

/**
 * Renders a Menuflyout component.
 *
 * This is a controlled component that requires an `onDismiss` callback that
 * updates the `isOpen` prop in order for the component to reflect user actions.
 *
 * @keyword menuFlyout
 */
export class MenuFlyout extends React.Component<IMenuFlyoutProps, IMenuFlyoutTargetState> {

  public static getDerivedStateFromProps(nextProps: IMenuFlyoutProps, prevState: IMenuFlyoutTargetState): IMenuFlyoutTargetState {
    // Check if we're given a new target property; we need to resolve it to a node handle before render
    if (prevState.targetRef !== nextProps.target) {
      // Map the 'target' property to a node tag to use in the native layer
      /* tslint:disable-next-line no-any */
      const newtarget: number | null = findNodeHandle(
        nextProps.target as null | number | React.Component<IMenuFlyoutProps, IMenuFlyoutTargetState> | React.ComponentClass<MenuFlyout>);

      return {
        target: newtarget,
        targetRef: nextProps.target,
      };
    }

    return prevState;
  }

  constructor(props: IMenuFlyoutProps) {
    super(props);
    this.state = { target: undefined, targetRef: null };
  }

  public render(): JSX.Element {
    const props = { ...this.props };

    return (
      <RCTMenuFlyout
        { ...props }
        target={ this.state.target }
        style={[styles.rctMenuFlyout, this.props.style]}
      />
    );
  }
}

export default MenuFlyout;
