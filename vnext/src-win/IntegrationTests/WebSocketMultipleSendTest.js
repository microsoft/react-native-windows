//TODO: Implement!

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
const {View} = ReactNative;
const {TestModule} = ReactNative.NativeModules;

const DEFAULT_WS_URL = 'ws://localhost:5555/rnw/websockets/echo';

const WS_EVENTS = ['close', 'error', 'message', 'open'];

type State = {
  url: string,
  fetchStatus: ?string,
  socket: ?WebSocket,
  socketState: ?number,
  lastSocketEvent: ?string,
  lastMessage: ?string | ?ArrayBuffer,
  testMessage: string,
  messageW: string,
  messageX: string,
  messageY: string,
  messageZ: string
  testExpectedResponse: string,
  expectedMessageCount: number,
  ...
};

class WebSocketMultpleSendTest extends React.Component<{...}, State> {
  state: State = {
    url: DEFAULT_WS_URL,
    fetchStatus: null,
    socket: null,
    socketState: null,
    lastSocketEvent: null,
    lastMessage: null,
    testMessage: 'testMessage',
    messageW: 'w'.repeat(1025),
    messageX: 'x'.repeat(1025),
    messageY: 'y'.repeat(1025),
    messageZ: 'z'.repeat(1025),
    testExpectedResponse: 'testMessage_response',
    expectedMessageCount: 4,
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

  _connect = () => {
    const socket = new WebSocket(this.state.url);
    WS_EVENTS.forEach(ev => socket.addEventListener(ev, this._onSocketEvent));
    this.setState({
      socket,
      socketState: socket.readyState,
    });
  };

  _socketIsConnected = (): boolean => {
    return this.state.socketState === 1; //'OPEN'
  };

  _socketIsDisconnected = (): boolean => {
    return this.state.socketState === 3; //'CLOSED'
  };

  _disconnect = () => {
    if (!this.state.socket) {
      return;
    }
    this.state.socket.close();
  };

  _onSocketEvent = (event: any) => {
    const state: any = {
      socketState: event.target.readyState,
      lastSocketEvent: event.type,
    };
    if (event.type === 'message') {
      state.lastMessage = event.data;
    }
    this.setState(state);
  };

  _sendText = (text: string) => {
    if (!this.state.socket) {
      return;
    }
    var w = 'w'.repeat(1024) + '.';
    var x = 'x'.repeat(1024) + '.';
    var y = 'y'.repeat(1024) + '.';
    var z = 'z'.repeat(1024) + '.';

    this.state.socket.send(w);
    this.state.socket.send(x);
    this.state.socket.send(y);
    this.state.socket.send(z);
  };

  _sendTestMessage = () => {
    this._sendText(this.state.testMessage);
  };

  _receivedTestExpectedResponse = (): boolean => {
    //return this.state.lastMessage === this.state.testExpectedResponse;
    return this.state.lastMessage.startsWith('z');
  };

  componentDidMount() {
    this.testConnect();
  }

  testConnect: () => void = () => {
    this._connect();
    this._waitFor(this._socketIsConnected, 5, connectSucceeded => {
      if (!connectSucceeded) {
        TestModule.markTestPassed(false);
        return;
      }
      //this.testSendAndReceive();
    });
  };

  testSendAndClose: () => void () => {
    
  };

  //testSendAndReceive: () => void = () => {
  //  this._sendTestMessage();
  //  this._waitFor(this._receivedTestExpectedResponse, 5, messageReceived => {
  //    if (!messageReceived) {
  //      TestModule.markTestPassed(false);
  //      return;
  //    }
  //    this.testDisconnect();
  //  });
  //};

  //testDisconnect: () => void = () => {
  //  this._disconnect();
  //  this._waitFor(this._socketIsDisconnected, 5, disconnectSucceeded => {
  //    TestModule.markTestPassed(disconnectSucceeded);
  //  });
  //};

  render(): React.Node {
    return <View />;
  }
}

WebSocketMultpleSendTest.displayName = 'WebSocketMultpleSendTest';

module.exports = WebSocketMultpleSendTest;
