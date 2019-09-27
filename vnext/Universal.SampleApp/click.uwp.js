/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 * @flow
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  Text,
  TouchableHighlight,
  View,
} from 'react-native';

export default class Bootstrap extends Component {
  state = {
    ticker: 0,
  };

  onSmallIncrement = () => {
    this.setState({ ticker: this.state.ticker + 1 });
  }

  onMediumIncrement = () => {
    this.setState({ ticker: this.state.ticker + 10 });
  }

  onLargeIncrement = () => {
    this.setState({ ticker: this.state.ticker + 100 });
  }

  render() {
    return (
      <View style={{ backgroundColor: 'blue', margin: 10, width: 120 }}
        acceptsKeyboardFocus={true}
        onClick={this.onLargeIncrement}>
        <TouchableHighlight style={{ backgroundColor: 'orange', margin: 15 }}
          acceptsKeyboardFocus={true}
          onPress={this.onMediumIncrement}>
          <TouchableHighlight style={{ backgroundColor: 'azure', margin: 15 }}
            onPress={this.onSmallIncrement}>
            <View style={{ margin: 5 }}>
              <Text>{this.state.ticker.toString()}</Text>
            </View>
          </TouchableHighlight>
        </TouchableHighlight>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
