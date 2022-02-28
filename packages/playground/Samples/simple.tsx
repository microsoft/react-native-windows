/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
import {AppRegistry, Text, View} from 'react-native';

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View
        accessible={true}
        style={{borderRadius: 30, width: 60, height: 160, margin: 10}}>
        <View style={{backgroundColor: 'black', width: 0.66, height: 30}} />
        <View style={{backgroundColor: 'magenta', width: 1, height: 30}} />
        <View style={{backgroundColor: 'green', width: 1.33, height: 32}} />
        <View style={{backgroundColor: 'red', width: 1.5, height: 30}} />
        <View style={{backgroundColor: 'blue', width: 2, height: 30}} />
        <Text style={{color: 'green'}}>Hello!</Text>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
