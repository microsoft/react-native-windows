/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {AppRegistry, Text, TouchableHighlight, View} from 'react-native';

export default class Bootstrap extends React.Component {
  state = {
    ticker: 0,
  };

  onSmallIncrement = () => {
    this.setState({ticker: this.state.ticker + 1});
  };

  onMediumIncrement = () => {
    this.setState({ticker: this.state.ticker + 10});
  };

  onLargeIncrement = () => {
    this.setState({ticker: this.state.ticker + 100});
  };

  render() {
    return (
      <View
        style={{backgroundColor: 'blue', margin: 10, width: 120}}
        {...{
          // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
          onClick: this.onLargeIncrement,
          acceptsKeyboardFocus: true,
        }}>
        <TouchableHighlight
          style={{backgroundColor: 'orange', margin: 15}}
          {...{
            // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
            onPress: this.onMediumIncrement,
            acceptsKeyboardFocus: true,
          }}>
          <TouchableHighlight
            style={{backgroundColor: 'azure', margin: 15}}
            onPress={this.onSmallIncrement}>
            <View style={{margin: 5}}>
              <Text>{this.state.ticker.toString()}</Text>
            </View>
          </TouchableHighlight>
        </TouchableHighlight>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
