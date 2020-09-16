/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

'use strict';

const React = require('react');
const ReactNative = require('react-native');
const {AppRegistry, View} = ReactNative;
const {TestModule} = ReactNative.NativeModules;

const DEFAULT_WS_URL = 'ws://localhost:5557/';

const WS_EVENTS = ['close', 'error', 'message', 'open'];

type State = {
  url: string,
  fetchStatus: ?string,
  socket: ?WebSocket,
  socketState: ?number,
  lastSocketEvent: ?string,
  lastMessage: ?ArrayBuffer,
  testMessage: ArrayBuffer,
  testExpectedResponse: ArrayBuffer,
  ...
};

class WebSocketBinaryTest extends React.Component<{...}, State> {
  state: State = {
    url: DEFAULT_WS_URL,
    fetchStatus: null,
    socket: null,
    socketState: null,
    lastSocketEvent: null,
    lastMessage: null,
    testMessage: new Uint8Array([1, 2, 3]).buffer,
    testExpectedResponse: new Uint8Array([4, 5, 6, 7]).buffer,
  };

  _waitFor = (condition: any, timeout: any, callback: any) => {
    let remaining = timeout;
    const timeoutFunction = function() {
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
    socket.binaryType = 'arraybuffer';
    WS_EVENTS.forEach(ev => socket.addEventListener(ev, this._onSocketEvent));
    this.setState({
      socket,
      socketState: socket.readyState,
    });
  };

  _socketIsConnected = () => {
    return this.state.socketState === 1; //'OPEN'
  };

  _socketIsDisconnected = () => {
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

  _sendBinary = (message: ArrayBuffer) => {
    if (!this.state.socket) {
      return;
    }
    this.state.socket.send(message);
  };

  _sendTestMessage = () => {
    this._sendBinary(this.state.testMessage);
  };

  _receivedTestExpectedResponse = () => {
    console.log(this.state.lastMessage);
    if (
      this.state.lastMessage?.byteLength !==
      this.state.testExpectedResponse.byteLength
    ) {
      return false;
    }

    var expected = new Uint8Array(this.state.testExpectedResponse);
    var result = this.state.lastMessage
      ? new Uint8Array(this.state.lastMessage)
      : new Uint8Array(expected.length);

    for (var i = 0; i < expected.length; i++) {
      if (expected[i] !== result[i]) {
        return false;
      }
    }

    return true;
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
      this.testSendAndReceive();
    });
  };

  testSendAndReceive: () => void = () => {
    this._sendTestMessage();
    this._waitFor(this._receivedTestExpectedResponse, 5, messageReceived => {
      if (!messageReceived) {
        TestModule.markTestPassed(false);
        return;
      }
      this.testDisconnect();
    });
  };

  testDisconnect: () => void = () => {
    this._disconnect();
    this._waitFor(this._socketIsDisconnected, 5, disconnectSucceeded => {
      TestModule.markTestPassed(disconnectSucceeded);
    });
  };

  render(): React.Node {
    return <View />;
  }
}

WebSocketBinaryTest.displayName = 'WebSocketBinaryTest';

AppRegistry.registerComponent('WebSocketBinaryTest', () => WebSocketBinaryTest);

module.exports = WebSocketBinaryTest;
