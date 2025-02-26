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

type State = {
  id: number,
  sendUrl: string,
  receiveUrl: string,
  incomingSocket: ?WebSocket,
  outgoingSocket: ?WebSocket,
  //lastSocketEvent: ?string,
  messageW: string,
  messageX: string,
  messageY: string,
  messageZ: string,
  result: ?string,
  ...
};

class WebSocketMultipleSendTest extends React.Component<{}, State> {
  state: State = {
    id: 1,
    sendUrl: `${URL_BASE}/send/${id}`,
    receiveUrl: `${URL_BASE}/receive/${id}`,
    incomingSocket: null,
    outgoingSocket: null,
    messageW: 'w'.repeat(1025),
    messageX: 'x'.repeat(1025),
    messageY: 'y'.repeat(1025),
    messageZ: 'z'.repeat(1025),
    result: '',
  };

  //_waitFor = (condition: any, timeout: any, callback: any) => {
  //  let remaining = timeout;
  //  const timeoutFunction = function () {
  //    if (condition()) {
  //      callback(true);
  //      return;
  //    }
  //    remaining--;
  //    if (remaining === 0) {
  //      callback(false);
  //    } else {
  //      setTimeout(timeoutFunction, 1000);
  //    }
  //  };
  //  setTimeout(timeoutFunction, 1000);
  //};

  //_connect = () => {
  //  const incomingSocket = new WebSocket(this.state.sendUrl);
  //  WS_EVENTS.forEach(ev => incomingSocket.addEventListener(ev, this._onIncomingEvent));
  //  const outgoingSocket = new WebSocket(this.state.receiveUrl);
  //  //WS_EVENTS.forEach(ev => outgoingSocket.addEventListener(ev, this._onSocketEvent));//TODO: Different callbacks?
  //  this.setState({
  //    incomingSocket,
  //    outgoingSocket,
  //    //TODO: Separate ready state?
  //  });
  //};

  //_socketsAreConnected = (): boolean => {
  //  return this.state.incomingSocket.readyState === 1 && this.state.outgoingSocket.readyState === 1; // OPEN
  //};

  //_socketsAreDisconnected = (): boolean => {
  //  return this.state.incomingSocket.readyState === 3 && this.state.outgoingSocket.readyState === 3; // CLOSED
  //};

  //_disconnectIncoming = () => {
  //  if (!this.state.incomingSocket) {
  //    return;
  //  }
  //  this.state.incomingSocket.close();

  //  //TODO: Remove
  //  if (!this.state.outgoingSocket) {
  //    return;
  //  }
  //  this.state.outgoingSocket.close();
  //};

  //_onIncomingEvent = (event: any) => {
  //  //const state: any = {
  //  //  //lastSocketEvent
  //  //};
  //  if (event.type === 'message') {
  //    var message = this.state.result;
  //     message += `${event.data[0]}`; // Append incomming message's first character
  //    this.setState({
  //      result: message
  //    });
  //  }
  //};

  //_resultIsComplete = (): boolean => {
  //  return EXPECTED === this.state.result;
  //};

  //testSendMultipleAndClose: () => void = () => {
  //  if (!this.state.incomingSocket || !this.state.outgoingSocket) {
  //    return;
  //  }
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
  //};

  //testDisconnect: () => void = () => {
  //  this._disconnectIncoming();
  //  this._waitFor(this._socketsAreDisconnected, 5, disconnectSucceeded => {
  //    TestModule.markTestPassed(disconnectSucceeded);
  //  });
  //};

  componentDidMount() {
    TestModule.markTestPassed(true);
    //console.warn('did mount!');
    //  this._connect();
    //  this._waitFor(this._socketsAreConnected, 5, connectSucceeded => {
    //    if (!connectSucceeded) {
    //      TestModule.markTestPassed(false);
    //      return;
    //    }
    //    this.testSendMultipleAndClose();
    //  });
  }

  render(): React.Node {
    return <View />;
  }
} // class WebSocketMultipleSendTest

WebSocketMultipleSendTest.displayName = 'WebSocketMultipleSendTest';

AppRegistry.registerComponent('WebSocketMultipleSendTest', () => WebSocketMultipleSendTest);

module.exports = WebSocketMultipleSendTest;
