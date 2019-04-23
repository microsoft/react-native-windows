/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 * @flow
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  Text,
  View,
} from 'react-native';

export default class Bootstrap extends Component {
  render() {
    return (
      <View accessible={true} style={{ borderRadius: 30, width: 60, height: 60, margin: 10 }}>
        <View style={{ backgroundColor: 'magenta', width: 60, height: 60 }} />
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
