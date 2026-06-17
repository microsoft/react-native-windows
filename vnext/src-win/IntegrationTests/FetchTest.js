/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

'use strict';

const React = require('react');
const ReactNative = require('react-native');

const {AppRegistry, View} = ReactNative;

const {TestModule} = ReactNative.NativeModules;

const uri =
  'https://raw.githubusercontent.com/microsoft/react-native-windows/main/.yarnrc.yml';
const expectedContent = 'enableScripts: false';

type State = {
  uri: string,
  expected: string,
  content: string,
};

class FetchTest extends React.Component<{...}, State> {
  state: State = {
    uri: 'https://raw.githubusercontent.com/microsoft/react-native-windows/main/.yarnrc.yml',
    expected: 'enableScripts: false',
    content: '',
  };

  async componentDidMount() {
    const response = await fetch(uri);
    const text = await response.text();
    this.setState({content: text});

    if (this.state.content === expectedContent) {
      TestModule.markTestPassed(true);
    } else {
      TestModule.markTestPassed(false);
    }
  }

  render(): React.Node {
    return <View />;
  }
}

AppRegistry.registerComponent('FetchTest', () => FetchTest);

module.exports = FetchTest;
