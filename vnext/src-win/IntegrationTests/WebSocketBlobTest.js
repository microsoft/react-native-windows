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
const TEST_MESSAGE = new Uint8Array([1, 2, 3]);
// Expected bytes come from RNTesterIntegrationTests.WebSocketBinaryTest outgoingBytes.
const EXPECTED_BYTES = new Uint8Array([4, 5, 6, 7]);
const EXPECTED_SIZE = EXPECTED_BYTES.length;

let completed = false;
const socket = new WebSocket(WS_URL);
socket.binaryType = 'blob';

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
    console.log('WebSocketBlobTest FAIL: ' + reason);
  }

  TestModule.markTestPassed(passed);
}

function decodeBlobToBytes(blob, onSuccess, onError) {
  if (typeof blob.arrayBuffer === 'function') {
    blob
      .arrayBuffer()
      .then(buffer => onSuccess(new Uint8Array(buffer)))
      .catch(onError);
    return;
  }

  const reader = new FileReader();
  reader.onload = () => {
    onSuccess(new Uint8Array(reader.result));
  };
  reader.onerror = () => {
    onError(reader.error || new Error('failed to read blob'));
  };
  reader.readAsArrayBuffer(blob);
}

socket.addEventListener('open', () => {
  socket.send(TEST_MESSAGE);
});

socket.addEventListener('message', event => {
  const data = event.data;

  if (!(data instanceof Blob)) {
    complete(false, 'expected Blob response');
    socket.close();
    return;
  }

  if (data.size !== EXPECTED_SIZE) {
    complete(false, 'unexpected response size');
    socket.close();
    return;
  }

  decodeBlobToBytes(
    data,
    bytes => {
      for (let i = 0; i < EXPECTED_BYTES.length; i++) {
        if (bytes[i] !== EXPECTED_BYTES[i]) {
          complete(
            false,
            'unexpected response byte at index ' +
              i +
              ': expected ' +
              EXPECTED_BYTES[i] +
              ', got ' +
              bytes[i],
          );
          socket.close();
          return;
        }
      }

      complete(true);
      socket.close();
    },
    error => {
      complete(false, 'failed to decode blob: ' + String(error));
      socket.close();
    },
  );
});

socket.addEventListener('error', () => {
  complete(false, 'websocket error');
});

socket.addEventListener('close', () => {
  if (!completed) {
    complete(false, 'socket closed before completing test');
  }
});
