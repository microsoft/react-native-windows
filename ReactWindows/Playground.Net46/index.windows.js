/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  View,
  TextInput,
  Alert
} from 'react-native';

class Playground extends Component {
  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome} testID='WelcomeText'>
          Welcome to React Native!
        </Text>
        <Text style={styles.instructions}>
          To get started, edit index.windows.js
        </Text>
        <Text style={styles.instructions}>
          Press Ctrl+R to reload
        </Text>
        <Text style={styles.instructions}>
          Press Ctrl+D or Ctrl+M for dev menu
        </Text>
        <TextInput multiline onSubmitEditing={(e) => Alert.alert('TextInput Submission', e.nativeEvent.text)} style={{width: 200, height: 200, backgroundColor: '#ffffff'}} />
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#F5FCFF'
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10
  },
  instructions: {
    textAlign: 'center',
    color: '#333333',
    marginBottom: 5
  },
});

AppRegistry.registerComponent('Playground.Net46', () => Playground);
