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

const {TestModule, BlobModule} = ReactNative.NativeModules;

type State = {
  statusCode: number,
  xhr: XMLHttpRequest,
};

class BlobTest extends React.Component<{...}, State> {
  state: State = {
    statusCode: 0,
    xhr: new XMLHttpRequest(),
  };

  _get = () => {
    this.state.xhr.onloadend = () => {
      this.setState({
        statusCode: this.state.xhr.status,
      });
    };
    this.state.xhr.open(
      'GET',
      //'https://www.facebook.com/favicon.ico?r=1&t=1652923053846',
      'http://localhost:666/hello.txt',
    );
    this.state.xhr.setRequestHeader('Accept-Encoding', 'utf-8');
    this.state.xhr.responseType = "blob";
    this.state.xhr.send();
  };

  _getSucceeded = () => {
    console.log(
      `_getSucceeded [${this.state.statusCode}],[${this.state.xhr.response}]`,
    );
    return (
      this.state.statusCode === 200 &&
      this.state.xhr.response !== null
    );
  };

  _waitFor = (condition: any, timeout: any, callback: any) => {
    let remaining = timeout;
    const timeoutFunction = function () {
      if (condition()) {
        callback(true);
        return;
      }
      remaining--;
      if (remaining === 0) {
        callback(false);
      } else {
        setTimeout(timeoutFunction, 1000);
      }
    };
    setTimeout(timeoutFunction, 1000);
  };

  componentDidMount() {
    this._get();
    this._waitFor(this._getSucceeded, 5, doneSucceeded => {
      console.log(`Suc: [${doneSucceeded}]`);
      TestModule.markTestPassed(doneSucceeded);
    });
  }

  render(): React.Node {
    return <View />;
  }
}

AppRegistry.registerComponent('BlobTest', () => BlobTest);

module.exports = BlobTest;
