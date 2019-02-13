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

const {
  TestModule
} = ReactNative.NativeModules;

class DummyTest extends React.Component {

  componentDidMount() {
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

AppRegistry.registerComponent('DummyTest', () => DummyTest);

module.exports = DummyTest;
