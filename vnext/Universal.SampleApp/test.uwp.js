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
  View,
} from 'react-native';

export default class Bootstrap extends Component {
  constructor(props) {
    super(props);
    this.state = { text: 'Starting text.' };
  }

  render() {
    return (
      <View style={styles.container}>
        <View style={styles.item}
            accessible={true}
            accessibilityLabel="THIRD ITEM"
            onFocus={() => this.setState({ text: 'FOCUSED' })}
            onBlur={() => this.setState({ text: 'BLURRED' })}>
          <Text style={styles.text}>{this.state.text}</Text>
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
    backgroundColor: 'lightgray',
  },
  item: {
    margin: 10,
    backgroundColor: 'lightpink',
    borderColor: 'indianred',
    borderWidth: 2,
    justifyContent: 'center',
  },
  text: {
    fontSize: 20,
    textAlign: 'center',
    color: 'black',
    margin: 10,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
