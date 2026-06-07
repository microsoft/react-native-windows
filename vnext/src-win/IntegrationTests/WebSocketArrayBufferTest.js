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
const WS_URL = 'ws://localhost:5555/rnw/rntester/websocketbinarytest';

const socket = new WebSocket(WS_URL);
socket.binaryType = 'arraybuffer';

socket.addEventListener('open', () => {
  socket.send('hello');
});

socket.addEventListener('message', event => {
  const data = event.data;

  if (!(data instanceof ArrayBuffer)) {
    console.log(
      'WebSocketArrayBufferTest FAIL: expected ArrayBuffer, got ' + typeof data,
    );
    TestModule.markTestPassed(false);
    socket.close();
    return;
  }

  const bytes = new Uint8Array(data);
  const expected = new Uint8Array([4, 5, 6, 7]);

  if (bytes.length !== expected.length) {
    console.log(
      'WebSocketArrayBufferTest FAIL: expected ' +
        expected.length +
        ' bytes, got ' +
        bytes.length,
    );
    TestModule.markTestPassed(false);
    socket.close();
    return;
  }

  for (let i = 0; i < expected.length; i++) {
    if (bytes[i] !== expected[i]) {
      console.log(
        'WebSocketArrayBufferTest FAIL: byte[' +
          i +
          '] expected ' +
          expected[i] +
          ' got ' +
          bytes[i],
      );
      TestModule.markTestPassed(false);
      socket.close();
      return;
    }
  }

  TestModule.markTestPassed(true);
  socket.close();
});

socket.addEventListener('error', () => {
  console.log('WebSocketArrayBufferTest FAIL: WebSocket error');
  TestModule.markTestPassed(false);
});
