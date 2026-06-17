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

type State = {
  statusCode: number,
  xhr: XMLHttpRequest,
  expected: string,
};

class BlobTest extends React.Component<{...}, State> {
  state: State = {
    statusCode: 0,
    xhr: new XMLHttpRequest(),
    // http://localhost:5555/static/react.png
    // cspell:disable
    expected:
      'data:application/octet-stream;base64,' +
      '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' +
      'uP2D2H8BsGL5W7blULYAAAAASUVORK5CYII=',
    // cspell:enable
  };

  _get = () => {
    this.state.xhr.onloadend = () => {
      this.setState({
        statusCode: this.state.xhr.status,
      });
    };
    this.state.xhr.open('GET', 'http://localhost:5555/static/react.png');
    this.state.xhr.setRequestHeader('Accept-Encoding', 'utf-8');
    this.state.xhr.responseType = 'blob';
    this.state.xhr.send();
  };

  _getSucceeded = (): boolean => {
    return this.state.statusCode === 200 && this.state.xhr.response !== null;
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
    this._waitFor(this._getSucceeded, 6, doneSucceeded => {
      let reader = new FileReader();
      reader.readAsDataURL(this.state.xhr.response);
      reader.onload = () => {
        TestModule.markTestPassed(
          doneSucceeded && this.state.expected === reader.result,
        );
      };
    });
  }

  render(): React.Node {
    return <View />;
  }
}

AppRegistry.registerComponent('BlobTest', () => BlobTest);

module.exports = BlobTest;
