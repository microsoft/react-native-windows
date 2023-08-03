/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

'use strict';

const React = require('react');
const ReactNative = require('react-native');

const { AppRegistry, View } = ReactNative;

const { TestModule } = ReactNative.NativeModules;

const uri = 'https://raw.githubusercontent.com/microsoft/react-native-windows/main/.yarnrc.yml';
const expectedContent = 'enableScripts: false';

class FetchTest extends React.Component {

  constructor() {
    super();
    this.state = { data: 'NONE' };
  }

  async componentDidMount() {
    const response = await fetch(uri);
    const text = await response.text();
    this.setState({ data: text });

    if (this.state.data === expectedContent) {
      TestModule.markTestPassed(true);
    } else {
      TestModule.markTestPassed(false);
    }

  }

  render() {
    return (
      <View />
    );
  }
}

AppRegistry.registerComponent('FetchTest', () => FetchTest);

module.exports = FetchTest;
