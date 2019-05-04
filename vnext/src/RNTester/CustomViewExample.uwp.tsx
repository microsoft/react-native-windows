// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { Button, NativeModules, StyleSheet, Text, View, ViewProps, findNodeHandle, requireNativeComponent } from 'react-native';


// JS to wrap the custom element.
// Typically you would export this class as the reusable component
const RCTCustomFrameworkElement = requireNativeComponent('RCTCustomFrameworkElement');
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

export class CustomFrameworkElement extends React.Component<ICustomFrameworkElementProps, {}> {
  private styles = StyleSheet.create({
    default: {
      height: 40,
      width: 40,
    },
  });

  public render() {
    const { onCustomEvent, ...rest }  = this.props;
    rest.style = [this.styles.default, this.props.style];

    return <RCTCustomFrameworkElement {...rest} onCustomEvent={this._onCustomEvent} />;
  }

  public exampleCommand() {
    NativeModules.UIManager.dispatchViewManagerCommand(findNodeHandle(this), NativeModules.UIManager.RCTCustomFrameworkElement.Commands.ExampleCommand, null);
  }

  private _onCustomEvent = (event: ICustomChangeEvent) => {
    this.props.onCustomEvent && this.props.onCustomEvent(event);
  }
}

// Example consuming the component
class CustomViewExample extends React.Component<{}, any> {
  private _customElementRef: any;

  public state = { countOfEvents: 0 };
  public constructor(props: {}) {
    super(props);
    this._customElementRef = React.createRef<CustomFrameworkElement>();
  }

  public render() {
    return (
      <View>
        <Text>A simple RCTCustomFrameworkElement:</Text>
        <CustomFrameworkElement disabled={false} test={true} onCustomEvent={this.onCustomEvent} ref={this._setRef} />
        <Text>count of events: {this.state.countOfEvents}</Text>
        <Button title="Run test command" onPress={this.onPress} />
      </View>
    );
  }

  private onCustomEvent = (event: ICustomChangeEvent) => {
    this.setState({countOfEvents: this.state.countOfEvents + 1 });
  }

  private onPress = () => {
    if (this._customElementRef) {
      this._customElementRef.exampleCommand();
    }
  }

  private _setRef = (customElement: any) => {
    this._customElementRef = customElement;
  }
}


export const displayName = (_undefined?: string) => {};
export const title = 'Custom Views';
export const description = 'Example of custom view managers.';
export const examples = [
  {
    title: 'Simple',
    render: function(): JSX.Element {
      return <CustomViewExample />;
    },
  }
];
