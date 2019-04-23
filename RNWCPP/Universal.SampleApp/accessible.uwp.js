/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 * @flow
 */

import React, { Component } from 'react';
import {
  AppRegistry,
  StyleSheet,
  Text,
  TextInput,
  View
} from 'react-native';

export default class Bootstrap extends Component {
  constructor(props) {
    super(props);
    this.state = { displayText: 'Starting text. (THIRD ITEM)' };
  }

  render() {
    return (
      <View style={styles.container}>
        <View style={styles.item} acceptsKeyboardFocus={true} accessibilityLabel="FIRST ITEM">
          <Text style={styles.text}>Welcome to React Native! (FIRST ITEM)</Text>
        </View>
        <View style={styles.item} acceptsKeyboardFocus={true} accessibilityLabel="SECOND ITEM" enableFocusRing={false}>
          <Text style={styles.text}>No focus visual (SECOND ITEM)</Text>
        </View>
        <View style={styles.item} acceptsKeyboardFocus={true} accessibilityLabel="THIRD ITEM"
          onFocus={() => this.setState({ displayText: 'FOCUSED' })}
          onBlur={() => this.setState({ displayText: 'BLURRED' })}>
          <Text style={styles.text}>{this.state.displayText}</Text>
        </View>
        <TextInput/>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
    backgroundColor: 'lightgray'
  },
  item: {
    margin: 10,
    backgroundColor: 'lightpink',
    borderColor: 'indianred',
    borderWidth: 2,
    justifyContent: 'center'
  },
  text: {
    fontSize: 20,
    textAlign: 'center',
    color: 'black',
    margin: 10
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
