// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

'use strict';

const React = require('react');
const ReactNative = require('react-native');

const {
  AppRegistry,
  StyleSheet,
  Text,
  View,
} = ReactNative;

const { TestModule } = ReactNative.NativeModules;

class LoggingTest extends React.Component {
  componentDidMount() {
    // Test the supported console logging methods
    console.trace('This is from console.trace'); // trace
    console.debug('This is from console.debug'); // trace
    console.info('This is from console.info'); // info
    console.log('This is from console.log'); // info
    console.warn('This is from console.warn'); // warn
    console.error('This is from console.error'); // error

    TestModule.markTestPassed(true);
  }

  render() {
    return (
      <View style={styles.container}>
        <Text style={styles.row}>Some text</Text>
      </View>
    );
  }
}

var styles = StyleSheet.create({
  container: {},
  row: {},
});

AppRegistry.registerComponent('LoggingTest', () => LoggingTest);

module.exports = LoggingTest;
