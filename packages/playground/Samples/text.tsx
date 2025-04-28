/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React from 'react';
import {AppRegistry, StyleSheet} from 'react-native';
import {Text, View} from 'react-native-windows';

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>Welcome to React Native!</Text>
        <Text
          style={styles.welcome}
          tooltip=" tooltip message for the text"
          accessible={true}
          selectable={true}>
          Click here : This is a text with a tooltip.
        </Text>
        <View 
          style={styles.container2}
          accessible={true}
          accessibilityLabel="Annotation Checkc"
          accessibilityAnnotation={{
            typeID: 'Comment',
            typeName:'Check Comment',
            author: 'Christopher tarantino',
            dateTime: '3/19/2025 1:03 PM',
          }}>
          <Text
            adjustsFontSizeToFit
            style={{maxHeight: 80, fontSize: 72}}
            minimumFontScale={0.5}>
            Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do
            eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim
            ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut
            aliquip ex ea commodo consequat.
          </Text>
        </View>
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
  container2: {
    backgroundColor: 'lightcoral',
    padding: 10,
    marginBottom: 10,
    width: 500,
    height: 100,
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
