/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {AppRegistry, StyleSheet, Text, View} from 'react-native';

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View>
        <Text style={styles.welcome}>Welcome to React Native!</Text>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#C5CCFF',
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
    backgroundColor: 'blue',
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
