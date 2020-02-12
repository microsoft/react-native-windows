/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

'use strict';

const React = require('react');
const ReactNative = require('react-native');
const {View} = ReactNative;

const {TestModule} = ReactNative.NativeModules;

class PropertiesUpdateTest extends React.Component {
  render() {
    if (this.props.markTestPassed) {
      TestModule.markTestPassed(true);
    }
    return <View />;
  }
}

PropertiesUpdateTest.displayName = 'PropertiesUpdateTest';

module.exports = PropertiesUpdateTest;
