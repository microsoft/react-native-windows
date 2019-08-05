/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {AppRegistry, Button, Text, View} from 'react-native';

import {
  CustomFrameworkElement,
  ICustomChangeEvent,
} from 'react-native-windows-extended';

// Example consuming the component
export default class Bootstrap extends React.Component<{}, any> {
  private _customElementRef: any;

  public state = {countOfEvents: 0};
  public constructor(props: {}) {
    super(props);
    this._customElementRef = React.createRef<CustomFrameworkElement>();
  }

  public render() {
    return (
      <View>
        <Text>A simple RCTCustomFrameworkElement:</Text>
        <CustomFrameworkElement
          disabled={false}
          test={true}
          onCustomEvent={this.onCustomEvent}
          ref={this._setRef}
        />
        <Text>count of events: {this.state.countOfEvents}</Text>
        <Button title="Run test command" onPress={this.onPress} />
      </View>
    );
  }

  private onCustomEvent = (_event: ICustomChangeEvent) => {
    this.setState({countOfEvents: this.state.countOfEvents + 1});
  };

  private onPress = () => {
    if (this._customElementRef) {
      this._customElementRef.exampleCommand();
    }
  };

  private _setRef = (customElement: any) => {
    this._customElementRef = customElement;
  };
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
