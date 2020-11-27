/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import * as React from 'react';
import {AppRegistry, NativeModules, StyleSheet, Text, View} from 'react-native';

export default class Bootstrap extends React.Component<
  {},
  {subject: string; msgs: string[]}
> {
  constructor(props: {}) {
    super(props);

    this.state = {
      subject: 'Loading...',
      msgs: Array(0),
    };

    NativeModules.ReadingPane.CurrentConversation(
      (js: {Subject: string; Messages: string[]}) =>
        this.loadedConversation(js),
    );
  }

  loadedConversation(js: {Subject: string; Messages: string[]}) {
    this.setState({
      subject: js.Subject,
      msgs: js.Messages,
    });
  }

  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>Welcome to React Native!</Text>
        <Text style={styles.instructions}>Subject: {this.state.subject}</Text>
        <Text style={styles.instructions}>
          To get started, edit index.windows.js
        </Text>
        <Text style={styles.instructions}>
          Press Cmd+R to reload,{'\n'}
          Cmd+D or shake for dev menu
        </Text>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#F5FCFF',
  },
  welcome: {
    fontSize: 20,
    textAlign: 'center',
    margin: 10,
  },
  instructions: {
    textAlign: 'center',
    color: '#333333',
    marginBottom: 5,
  },
});

AppRegistry.registerComponent('Bootstrap', () => Bootstrap);
