/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

const {TurboModuleRegistry} = require('react-native');
const TestModule = TurboModuleRegistry.get('TestModule');

if (!TestModule) {
  throw new Error('TestModule is not available');
}

// eslint-disable-next-line @microsoft/sdl/no-insecure-url
const WS_URL = 'ws://localhost:5555/';
const TEST_MESSAGE = 'testMessage';
const EXPECTED_RESPONSE = 'testMessage_response';

let completed = false;
const socket = new WebSocket(WS_URL);
const timeoutId = setTimeout(() => {
  complete(false, 'timeout waiting for websocket response');
}, 10000);

function complete(passed, reason) {
  if (completed) {
    return;
  }

  completed = true;
  clearTimeout(timeoutId);

  if (!passed && reason) {
    console.log('WebSocketTest FAIL: ' + reason);
  }

  TestModule.markTestPassed(passed);
}

socket.addEventListener('open', () => {
  socket.send(TEST_MESSAGE);
});

socket.addEventListener('message', event => {
  if (event.data !== EXPECTED_RESPONSE) {
    complete(false, 'unexpected response payload');
    socket.close();
    return;
  }

  complete(true);
  socket.close();
});

socket.addEventListener('error', () => {
  complete(false, 'websocket error');
});

socket.addEventListener('close', () => {
  if (!completed) {
    complete(false, 'socket closed before completing test');
  }
});
