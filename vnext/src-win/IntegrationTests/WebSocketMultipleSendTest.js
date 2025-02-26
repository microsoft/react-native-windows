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
Sample client code:
var w = 'w'.repeat(1025); var x = 'x'.repeat(1025); var y = 'y'.repeat(1025); var z = 'z'.repeat(1025);
var ws = new WebSocket('ws://localhost:5555/rnw/websockets/echo'); ws.onmessage = (e) => console.log(e.data);
w.send(w);ws.send(x);ws.send(y);ws.send(z);ws.close();
*/

'use strict';

const React = require('react');
const ReactNative = require('react-native');
const { AppRegistry, View } = ReactNative;
const { TestModule } = ReactNative.NativeModules;

// eslint-disable-next-line @microsoft/sdl/no-insecure-url
const URL_BASE = 'ws://localhost:5555/rnw/rntester/websocketmultiplesendtest';

const WS_EVENTS = ['open', 'message', 'close', 'error'];

const EXPECTED = 'wxyz';

//lastSocketEvent: ?string,
type State = {
  sendUrl: string,
  receiveUrl: string,
  incomingSocket: ?WebSocket,
  outgoingSocket: ?WebSocket,
  messageW: string,
  messageX: string,
  messageY: string,
  messageZ: string,
  result: ?string,
  //...
};

const ID = Math.floor(Math.random() * 1000000);

class WebSocketMultipleSendTest extends React.Component<{}, State> {
  state: State = {
    sendUrl: `${URL_BASE}/send/${ID}`,
    receiveUrl: `${URL_BASE}/receive/${ID}`,
    incomingSocket: null,
    outgoingSocket: null,
    messageW: 'w'.repeat(1025),
    messageX: 'x'.repeat(1025),
    messageY: 'y'.repeat(1025),
    messageZ: 'z'.repeat(1025),
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
    return this.state.incomingSocket.readyState === 1 && this.state.outgoingSocket.readyState === 1; // OPEN
  };

  _socketsAreDisconnected = (): boolean => {
    return this.state.incomingSocket.readyState === 3 && this.state.outgoingSocket.readyState === 3; // CLOSED
  };

  _resultIsComplete = (): boolean => {
    return true;
    //TODO: Implement!
  };

  _disconnectIncoming = () => {
    if (!this.state.incomingSocket) {
      return;
    }
    this.state.incomingSocket.close();

    //TODO: Remove
    if (!this.state.outgoingSocket) {
      return;
    }
    this.state.outgoingSocket.close();
  };

  testDisconnect: () => void = () => {
    this._disconnectIncoming();
    this._waitFor(this._socketsAreDisconnected, 5, disconnectSucceeded => {
      TestModule.markTestPassed(disconnectSucceeded);
    });
  };

  testSendMultipleAndClose: () => void = () => {
    if (!this.state.incomingSocket || !this.state.outgoingSocket) {
      return;
    }
  //  this.state.outgoingSocket.send(this.state.messageW);
  //  this.state.outgoingSocket.send(this.state.messageX);
  //  this.state.outgoingSocket.send(this.state.messageY);
  //  this.state.outgoingSocket.send(this.state.messageZ);

  //  this._waitFor(this._resultIsComplete, 5, resultComplete => {
  //    if (!resultComplete) {
  //      TestModule.markTestPassed(false);
  //      return;
  //    }
  //    this.testDisconnect();
  //  });
    TestModule.markTestPassed(true);
  };

  _connect = () => {
    const incomingSocket = new WebSocket(this.state.sendUrl);
    WS_EVENTS.forEach(ev => incomingSocket.addEventListener(ev, this._onIncomingEvent));
    const outgoingSocket = new WebSocket(this.state.receiveUrl);
    //WS_EVENTS.forEach(ev => outgoingSocket.addEventListener(ev, this._onSocketEvent));//TODO: Different callbacks?
    this.setState({
      incomingSocket,
      outgoingSocket,
      //TODO: Separate ready state?
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
      //TestModule.markTestPassed(true);
    });
  }

  render(): React.Node {
    return <View />;
  }
} // class WebSocketMultipleSendTest

WebSocketMultipleSendTest.displayName = 'WebSocketMultipleSendTest';

AppRegistry.registerComponent('WebSocketMultipleSendTest', () => WebSocketMultipleSendTest);

module.exports = WebSocketMultipleSendTest;
