/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {AppRegistry, Text, TouchableHighlight, View} from 'react-native';

export default class Bootstrap extends React.Component {
  state = {
    ticker: 0,
  };

  onSmallIncrement = () => {
      this.setState({ ticker: this.state.ticker + 1 });
      console.log(" onSmallIncrement !");
  };

  onMediumIncrement = () => {
      this.setState({ ticker: this.state.ticker + 10 });
      console.log(" onMediumIncrement !");
  };

  onLargeIncrement = () => {
      this.setState({ ticker: this.state.ticker + 100 });
      console.log(" onLargeIncrement !");
  };

  render() {
    return (
      <View
        style={{backgroundColor: 'blue', margin: 10, width: 120}}
        focusable
        {...{
          // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
          onClick: this.onLargeIncrement,
        }}>
        <TouchableHighlight
          style={{backgroundColor: 'orange', margin: 15}}
          onPress={this.onMediumIncrement}
          {...{
            // Use weird format as work around for the fact that these props are not part of the @types/react-native yet
            focusable: true,
          }}>
          <TouchableHighlight
            style={{backgroundColor: 'azure', margin: 15}}
            onPress={this.onSmallIncrement}>
            <View style={{margin: 5}}>
              <Text style={{color: 'black'}}>
                {this.state.ticker.toString()}
              </Text>
            </View>
          </TouchableHighlight>
        </TouchableHighlight>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
