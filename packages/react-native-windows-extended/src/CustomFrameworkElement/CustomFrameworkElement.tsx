/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {
  NativeModules,
  StyleSheet,
  ViewProps,
  findNodeHandle,
  requireNativeComponent,
} from 'react-native';

// JS to wrap the custom element.
// Typically you would export this class as the reusable component
const RCTCustomFrameworkElement = requireNativeComponent(
  'RCTCustomFrameworkElement',
);
export interface ICustomFrameworkElementProps extends ViewProps {
  disabled?: boolean;
  test: boolean;
  onCustomEvent?: (event: ICustomChangeEvent) => void;
}

export interface ICustomChangeEvent {
  nativeEvent: {
    value: boolean;
  };
}

export class CustomFrameworkElement extends React.Component<
  ICustomFrameworkElementProps,
  {}
> {
  private styles = StyleSheet.create({
    default: {
      height: 40,
      width: 40,
    },
  });

  public render() {
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    const {onCustomEvent, ...rest} = this.props;
    rest.style = [this.styles.default, this.props.style];

    return (
      <RCTCustomFrameworkElement
        {...rest}
        onCustomEvent={this._onCustomEvent}
      />
    );
  }

  public exampleCommand() {
    NativeModules.UIManager.dispatchViewManagerCommand(
      findNodeHandle(this),
      NativeModules.UIManager.getViewManagerConfig('RCTCustomFrameworkElement')
        .Commands.ExampleCommand,
      null,
    );
  }

  private _onCustomEvent = (event: ICustomChangeEvent) => {
    this.props.onCustomEvent && this.props.onCustomEvent(event);
  };
}
