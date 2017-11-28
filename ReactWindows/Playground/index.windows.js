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
} from 'react-native';

require('react-native/Libraries/BatchedBridge/MessageQueue').spy(info => {
  console.log(`${!info.type ? 'N->JS' : 'JS->N'} : `
    + `${info.module ? info.module + '.' : ''}${info.method}`
    + `(${JSON.stringify(info.args)})`);
});

class Playground extends Component {
  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.welcome}>
          Welcome to React Native!
        </Text>
        <Text style={styles.instructions}>
          To get started, edit index.windows.js
        </Text>
        <Text style={styles.instructions}>
          Shake or press Shift+F10 for dev menu
        </Text>
        <View style={styles.droptarget} allowDrop={true}
          onDragEnter={e => console.log('onDragEnter', e.nativeEvent)}
          onDragOver={e => console.log('onDragOver', e.nativeEvent)}
          onDrop={e => console.log('onDrop', e.nativeEvent)}
          onDragLeave={e => console.log('onDragLeave', e.nativeEvent)}
        >
          <Text>drop files here</Text>
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
  droptarget: {
    backgroundColor: '#cfc',
    borderColor: '#484',
    borderWidth: 2,
    borderRadius: 5,
    padding: 20,
  },
});

AppRegistry.registerComponent('Playground', () => Playground);
