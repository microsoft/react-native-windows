/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

/*
Sample client code
The server should successfully receive all messsges in order.

var id = Math.floor(Math.random() * 1000000);
var w = 'w'.repeat(1025); var x = 'x'.repeat(1025); var y = 'y'.repeat(1025); var z = 'z'.repeat(1025);
var ws = new WebSocket(`ws://localhost:5555/rnw/rntester/websocketmultiplesendtest/send/${id}`); ws.onmessage = (e) => console.log(e.data);
ws.send(w);ws.send(x);ws.send(y);ws.send(z);ws.close();
*/

'use strict';

const React = require('react');
const ReactNative = require('react-native');
const { AppRegistry, View } = ReactNative;
const { TestModule } = ReactNative.NativeModules;

// eslint-disable-next-line @microsoft/sdl/no-insecure-url
const URL_BASE = 'ws://localhost:5555/rnw/rntester/websocketmultiplesendtest';

const WS_EVENTS = ['open', 'message', 'close', 'error'];

const MESSAGE_SIZE = 16;//TODO: Bump?

const EXPECTED = 'abcdef';

const ID = Math.floor(Math.random() * 1000000);

type State = {
  sendUrl: string,
  receiveUrl: string,
  sendSocket: ?WebSocket,
  receiveSocket: ?WebSocket,
  messageA: string,
  messageB: string,
  messageC: string,
  messageD: string,
  messageE: string,
  messageF: string,
  result: ?string,
};

class WebSocketMultipleSendTest extends React.Component<{}, State> {
  state: State = {
    sendUrl: `${URL_BASE}/send/${ID}`,
    receiveUrl: `${URL_BASE}/receive/${ID}`,
    sendSocket: null,
    receiveSocket: null,
    messageA: 'a'.repeat(MESSAGE_SIZE),
    messageB: 'b'.repeat(MESSAGE_SIZE),
    messageC: 'c'.repeat(MESSAGE_SIZE),
    messageD: 'd'.repeat(MESSAGE_SIZE),
    messageE: 'e'.repeat(MESSAGE_SIZE),
    messageF: 'f'.repeat(MESSAGE_SIZE),
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
    return this.state.sendSocket.readyState === 1 && this.state.receiveSocket.readyState === 1; // OPEN
  };

  _socketsAreDisconnected = (): boolean => {
    return this.state.sendSocket.readyState === 3 && this.state.receiveSocket.readyState === 3; // CLOSED
  };

  _resultIsComplete = (): boolean => {
    return this.state.result === EXPECTED;
  };

  _disconnect = () => {
    //TODO: Remove
    //if (this.state.sendSocket) {
    //  this.state.sendSocket.close();
    //}

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
    if (!this.state.sendSocket || !this.state.receiveSocket) {
      return;
    }
    //this.state.sendSocket.send(this.state.messageA);
    //this.state.sendSocket.send(this.state.messageB);
    //this.state.sendSocket.send(this.state.messageC);
    //this.state.sendSocket.send(this.state.messageD);
    //this.state.sendSocket.send(this.state.messageE);
    //this.state.sendSocket.send(this.state.messageF);
    this.state.sendSocket.send('a'.repeat(MESSAGE_SIZE));
    this.state.sendSocket.send('b'.repeat(MESSAGE_SIZE));
    this.state.sendSocket.send('c'.repeat(MESSAGE_SIZE));
    this.state.sendSocket.send('d'.repeat(MESSAGE_SIZE));
    this.state.sendSocket.send('e'.repeat(MESSAGE_SIZE));
    this.state.sendSocket.send('f'.repeat(MESSAGE_SIZE));
    this.state.sendSocket.close();

    this._waitFor(this._resultIsComplete, 5, resultComplete => {
      if (!resultComplete) {
        TestModule.markTestPassed(false);
        return;
      }
      this.testDisconnect();
    });
  };

  _onSocketEvent = (event: any) => {
    if (event.type === 'message') {
      var message = this.state.result + event.data[0];
      this.setState({
        result: message,
      });
    }
  };

  _connect = () => {
    const sendSocket = new WebSocket(this.state.sendUrl);
    const receiveSocket = new WebSocket(this.state.receiveUrl);
    WS_EVENTS.forEach(ev => receiveSocket.addEventListener(ev, this._onSocketEvent));
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

AppRegistry.registerComponent('WebSocketMultipleSendTest', () => WebSocketMultipleSendTest);

module.exports = WebSocketMultipleSendTest;
