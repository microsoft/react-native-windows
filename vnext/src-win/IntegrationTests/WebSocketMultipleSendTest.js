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

const {TurboModuleRegistry} = require('react-native');

const TestModule = TurboModuleRegistry.get('TestModule');

if (!TestModule) {
  throw new Error('TestModule is not available');
}

// eslint-disable-next-line @microsoft/sdl/no-insecure-url
const URL_BASE = 'ws://localhost:5555/rnw/rntester/websocketmultiplesendtest';

const WS_EVENTS = ['open', 'message', 'close', 'error'];

const MESSAGE_SIZE = 16;

const EXPECTED = 'abcdef';

const ID = Math.floor(Math.random() * 1000000);

const sendUrl = `${URL_BASE}/send/${ID}`;
const receiveUrl = `${URL_BASE}/receive/${ID}`;

let sendSocket = null;
let receiveSocket = null;
let result = '';

const waitFor = (condition, timeout, callback) => {
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

const socketsAreConnected = () => {
  return sendSocket?.readyState === 1 && sendSocket?.readyState === 1; // OPEN
};

const socketsAreDisconnected = () => {
  return sendSocket?.readyState === 3 && sendSocket?.readyState === 3; // CLOSED
};

const resultIsComplete = () => {
  return result === EXPECTED;
};

const disconnect = () => {
  if (receiveSocket) {
    receiveSocket.close();
  }
};

const testDisconnect = () => {
  disconnect();
  waitFor(socketsAreDisconnected, 5, disconnectSucceeded => {
    TestModule.markTestPassed(disconnectSucceeded);
  });
};

const testSendMultipleAndClose = () => {
  sendSocket?.send('a'.repeat(MESSAGE_SIZE));
  sendSocket?.send('b'.repeat(MESSAGE_SIZE));
  sendSocket?.send('c'.repeat(MESSAGE_SIZE));
  sendSocket?.send('d'.repeat(MESSAGE_SIZE));
  sendSocket?.send('e'.repeat(MESSAGE_SIZE));
  sendSocket?.send('f'.repeat(MESSAGE_SIZE));
  sendSocket?.close();

  waitFor(resultIsComplete, 5, resultComplete => {
    if (!resultComplete) {
      TestModule.markTestPassed(false);
      return;
    }
    testDisconnect();
  });
};

const onSocketEvent = event => {
  if (event.type === 'message' && event.data.length) {
    result += event.data[0];
  }
};

const connect = () => {
  sendSocket = new WebSocket(sendUrl);
  receiveSocket = new WebSocket(receiveUrl);
  WS_EVENTS.forEach(ev => receiveSocket.addEventListener(ev, onSocketEvent));
};

connect();
waitFor(socketsAreConnected, 5, connectSucceeded => {
  if (!connectSucceeded) {
    TestModule.markTestPassed(false);
    return;
  }
  testSendMultipleAndClose();
});
