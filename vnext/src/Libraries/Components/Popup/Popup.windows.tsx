/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import * as React from 'react';
import {findNodeHandle, requireNativeComponent, StyleSheet} from 'react-native';
import {IPopupProps} from './PopupProps';

const styles = StyleSheet.create({
  rctPopup: {
    position: 'absolute',
  },
});

export interface IPopupTargetState {
  target?: number | null;
  targetRef?: React.ReactNode;
}

const RCTPopup = requireNativeComponent('RCTPopup');

/**
 * Renders a popup component.
 *
 * This is a controlled component that requires an `onDismiss` callback that
 * updates the `isOpen` prop in order for the component to reflect user actions.
 *
 * @keyword popup
 */
export class Popup extends React.Component<IPopupProps, IPopupTargetState> {
  public static getDerivedStateFromProps(
    nextProps: IPopupProps,
    prevState: IPopupTargetState,
  ): IPopupTargetState {
    // Check if we're given a new target property; we need to resolve it to a node handle before render
    if (prevState.targetRef !== nextProps.target) {
      // Map the 'target' property to a node tag to use in the native layer
      /* tslint:disable-next-line no-any */
      const newTarget: number | null = findNodeHandle(nextProps.target as
        | null
        | number
        | React.Component<IPopupProps, IPopupTargetState>
        | React.ComponentClass<Popup>);

      return {
        target: newTarget,
        targetRef: nextProps.target,
      };
    }

    return prevState;
  }

  constructor(props: IPopupProps) {
    super(props);
    this.state = {target: undefined, targetRef: null};
  }

  public render(): JSX.Element {
    const props = {...this.props};
    props.style = [styles.rctPopup, this.props.style];

    return <RCTPopup {...props} target={this.state.target} />;
  }
}

export default Popup;
