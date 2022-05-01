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
  lastMessage: ?Blob,
  testMessage: Uint8Array,
  testExpectedResponse: Uint8Array,
  ...
};

class WebSocketBlobTest extends React.Component<{}, State> {
  state: State = {
    url: DEFAULT_WS_URL,
    fetchStatus: null,
    socket: null,
    socketState: null,
    lastSocketEvent: null,
    lastMessage: null,
    testMessage: new Uint8Array([1, 2, 3]),
    testExpectedResponse: new Uint8Array([4, 5, 6, 7]),
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
    socket.binaryType = 'blob';
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

  _sendBinary = (message: Uint8Array) => {
    if (!this.state.socket) {
      return;
    }
    this.state.socket.send(message);
  };

  _sendTestMessage = () => {
    this._sendBinary(this.state.testMessage);
  };

  _receivedTestExpectedResponse = () => {
    // Can't iterate through Blob response. Blob.arrayBuffer() not supported.
    return (
      this.state.lastMessage?.size === this.state.testExpectedResponse.length
    );
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
} // class WebSocketBlobTest

WebSocketBlobTest.displayName = 'WebSocketBlobTest';

AppRegistry.registerComponent('WebSocketBlobTest', () => WebSocketBlobTest);

module.exports = WebSocketBlobTest;
