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
      <View accessible={true} style={{ flex: 1, justifyContent: 'center', alignItems: 'center', borderColor: 'red', borderWidth: 5, backgroundColor: 'orange' }}>
        <Text style={{ fontSize: 20, textAlign: 'center', margin: 10 }}>Welcome to React Native! X</Text>
      </View>
    );
  }
}

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
