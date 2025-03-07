/**
 * Copyright (c) Microsoft Corporation.
 *
 * Licensed under the MIT License.
 *
 *
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

/*
Sample client code (i.e. Node)
The server should successfully receive all messsges in order.

var id = Math.floor(Math.random() * 1000000);
var ws = new WebSocket(`ws://localhost:5555/rnw/rntester/websocketmultiplesendtest/send/${id}`);
ws.onmessage = (e) => console.log(e.data);
Array.from('abcdef').forEach( e => { ws.send(e.repeat(1025)) });
*/

'use strict';

const React = require('react');
const ReactNative = require('react-native');
const {AppRegistry, View} = ReactNative;
const {TestModule} = ReactNative.NativeModules;

// eslint-disable-next-line @microsoft/sdl/no-insecure-url
const URL_BASE = 'ws://localhost:5555/rnw/rntester/websocketmultiplesendtest';

const WS_EVENTS = ['open', 'message', 'close', 'error'];

const MESSAGE_SIZE = 16;

const EXPECTED = 'abcdef';

const ID = Math.floor(Math.random() * 1000000);

type State = {
  sendUrl: string,
  receiveUrl: string,
  sendSocket: ?WebSocket,
  receiveSocket: ?WebSocket,
  result: ?string,
};

class WebSocketMultipleSendTest extends React.Component<{}, State> {
  state: State = {
    sendUrl: `${URL_BASE}/send/${ID}`,
    receiveUrl: `${URL_BASE}/receive/${ID}`,
    sendSocket: null,
    receiveSocket: null,
    result: '',
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

  _socketsAreConnected = (): boolean => {
    return (
      this.state.sendSocket?.readyState === 1 &&
      this.state.sendSocket?.readyState === 1
    ); // OPEN
  };

  _socketsAreDisconnected = (): boolean => {
    return (
      this.state.sendSocket?.readyState === 3 &&
      this.state.sendSocket?.readyState === 3
    ); // CLOSED
  };

  _resultIsComplete = (): boolean => {
    return this.state.result === EXPECTED;
  };

  _disconnect = () => {
    if (this.state.receiveSocket) {
      this.state.receiveSocket.close();
    }
  };

  testDisconnect: () => void = () => {
    this._disconnect();
    this._waitFor(this._socketsAreDisconnected, 5, disconnectSucceeded => {
      TestModule.markTestPassed(disconnectSucceeded);
    });
  };

  testSendMultipleAndClose: () => void = () => {
    this.state.sendSocket?.send('a'.repeat(MESSAGE_SIZE));
    this.state.sendSocket?.send('b'.repeat(MESSAGE_SIZE));
    this.state.sendSocket?.send('c'.repeat(MESSAGE_SIZE));
    this.state.sendSocket?.send('d'.repeat(MESSAGE_SIZE));
    this.state.sendSocket?.send('e'.repeat(MESSAGE_SIZE));
    this.state.sendSocket?.send('f'.repeat(MESSAGE_SIZE));
    this.state.sendSocket?.close();

    this._waitFor(this._resultIsComplete, 5, resultComplete => {
      if (!resultComplete) {
        TestModule.markTestPassed(false);
        return;
      }
      this.testDisconnect();
    });
  };

  _onSocketEvent = (event: any) => {
    if (event.type === 'message' && event.data.length) {
      var message = this.state.result + event.data[0];
      this.setState({
        result: message,
      });
    }
  };

  _connect = () => {
    const sendSocket = new WebSocket(this.state.sendUrl);
    const receiveSocket = new WebSocket(this.state.receiveUrl);
    WS_EVENTS.forEach(ev =>
      receiveSocket.addEventListener(ev, this._onSocketEvent),
    );
    this.setState({
      sendSocket,
      receiveSocket,
    });
  };

  componentDidMount() {
    this._connect();
    this._waitFor(this._socketsAreConnected, 5, connectSucceeded => {
      if (!connectSucceeded) {
        TestModule.markTestPassed(false);
        return;
      }
      this.testSendMultipleAndClose();
    });
  }

  render(): React.Node {
    return <View />;
  }
} // class WebSocketMultipleSendTest

WebSocketMultipleSendTest.displayName = 'WebSocketMultipleSendTest';

AppRegistry.registerComponent(
  'WebSocketMultipleSendTest',
  () => WebSocketMultipleSendTest,
);

module.exports = WebSocketMultipleSendTest;
