/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
import React from 'react';
import {
  AppRegistry,
  View,
  Text,
  TextInput,
  Button,
  StyleSheet,
} from 'react-native';

export default class Bootstrap extends React.Component {
  render() {
    return (
      <View>
        <Text>TextInput Placeholder Text</Text>
        <TextInput
          style={styles.singleLine}
          placeholder="I am a placeholder text"
          placeholderTextColor={'green'}
        />
        <TextInput
          style={styles.singleLine}
          placeholder="I am a placeholder text2"
          placeholderTextColor={'green'}
        />
        <TextInput
          style={styles.singleLine}
          placeholder="I am a placeholder text3"
          placeholderTextColor={'green'}
        />
        <TextInput
          style={styles.singleLine}
          placeholder="I am a placeholder text4"
          placeholderTextColor={'green'}
        />
      </View>
    );
  }
}

const styles = StyleSheet.create({
  default: {
    borderWidth: StyleSheet.hairlineWidth,
    borderColor: '#0f0f0f',
    flex: 1,
    fontSize: 13,
    padding: 4,
  },
  multiline: {
    borderWidth: StyleSheet.hairlineWidth,
    borderColor: '#0f0f0f',
    flex: 1,
    fontSize: 13,
    height: 50,
    padding: 4,
    marginBottom: 4,
  },
  singleLine: {
    fontSize: 16,
    borderWidth: StyleSheet.hairlineWidth,
    borderColor: '#0f0f0f',
  },
  labelContainer: {
    flexDirection: 'row',
    marginVertical: 2,
  },
  label: {
    width: 115,
    textAlign: 'right',
    marginRight: 10,
    paddingTop: 2,
    fontSize: 12,
  },
  inputContainer: {
    flex: 1,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
