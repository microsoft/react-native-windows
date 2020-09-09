/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Socket} from 'net';
import {execSync} from 'child_process';

import * as psList from 'ps-list';

import TestWebSocketServer from './TestWebSocketServer';

async function connectToHarness(): Promise<Socket> {
  return new Promise((resolve, reject) => {
    const socket = new Socket();

    const onError = (err: Error) => reject(err);
    socket.on('error', onError);

    socket.connect(30977, '127.0.0.1', () => {
      socket.off('error', onError);
      resolve(socket);
    });
  });
}

function sendTestCommand(
  socket: Socket,
  command: Record<string, any>,
): Promise<Record<string, any>> {
  return new Promise((resolve, reject) => {
    let receiveBuffer = Buffer.alloc(0);

    const onError = (err: Error) => reject(err);

    const onData = (chunk: Buffer) => {
      receiveBuffer = Buffer.concat([receiveBuffer, chunk]);
      if (receiveBuffer.length >= 4) {
        const messageLength = receiveBuffer.readUInt32LE();
        const totalLength = messageLength + 4;

        if (receiveBuffer.length >= totalLength) {
          const res = receiveBuffer.toString('utf8', 4);
          resolve(JSON.parse(res));

          socket.off('data', onData);
          socket.off('error', onError);
        }
      }
    };

    socket.on('data', onData);
    socket.on('error', onError);

    const payload = JSON.stringify(command);
    const sizeBuffer = Buffer.alloc(4);
    sizeBuffer.writeUInt32LE(payload.length);

    socket.write(sizeBuffer);
    socket.write(Buffer.from(payload));
  });
}

async function startLoopbackServerExemption() {
  const processes = await psList();
  if (!processes.some(p => p.name === 'CheckNetIsolation.exe')) {
    execSync(
      'powershell Start-Process CheckNetIsolation.exe -ArgumentList "LoopbackExempt","-is" -Verb runAs',
    );
  }
}

async function loadTestComponent(socket: Socket, componentName: string) {
  const response = await sendTestCommand(socket, {
    command: 'RunTestComponent',
    component: componentName,
  });

  expect(response).toEqual({status: 'okay', passed: true});
}

let socket: Socket;
let websocketServer: TestWebSocketServer;

beforeAll(async () => {
  await startLoopbackServerExemption();
  socket = await connectToHarness();
  websocketServer = new TestWebSocketServer();

  // Go to a component away from tests so that everything gets correctly
  // reloaded.
  await sendTestCommand(socket, {
    command: 'GoToComponent',
    component: 'IntegrationTestsApp',
  });
});

afterAll(async () => {
  socket.destroy();
  websocketServer.close();
});

test('IntegrationTestHarnessTest', async () =>
  await loadTestComponent(socket, 'IntegrationTestHarnessTest'));

// This one is seemingly broken upstream since
// https://github.com/facebook/react-native/commit/61346d303a44b643b6029835018518d48c285a69
test.skip('TimersTest', async () =>
  await loadTestComponent(socket, 'TimersTest'));

// Our builtin implementation doens't implement mergeItems
test.skip('AsyncStorageTest', async () =>
  await loadTestComponent(socket, 'AsyncStorageTest'));

// ref.measure returns bad results for us (#5934)
test.skip('LayoutEventsTest', async () =>
  await loadTestComponent(socket, 'LayoutEventsTest'));

test('AppEventsTest', async () =>
  await loadTestComponent(socket, 'AppEventsTest'));

// Native snapshot checks not implemented
test.skip('SimpleSnapshotTest', async () =>
  await loadTestComponent(socket, 'SimpleSnapshotTest'));

// Image cache control is currently iOS only
test.skip('ImageCachePolicyTest', async () =>
  await loadTestComponent(socket, 'ImageCachePolicyTest'));

// Native snapshot checks not implemented
test.skip('ImageSnapshotTest', async () =>
  await loadTestComponent(socket, 'ImageSnapshotTest'));

test('PromiseTest', async () => await loadTestComponent(socket, 'PromiseTest'));

// Skip SyncMethodTest because we don't implement RNTesterTestModule and want
// to allow web debugging
test.skip('SyncMethodTest', async () =>
  await loadTestComponent(socket, 'SyncMethodTest'));

test('WebSocketTest', async () =>
  await loadTestComponent(socket, 'WebSocketTest'));

// We implement the Android AccessibilityInfo instead of AccessibilityManager
test.skip('AccessibilityManagerTest', async () =>
  await loadTestComponent(socket, 'AccessibilityManagerTest'));

// globalEvalWithSourceUrl is only set when using JSIExecutor in debug. I.e. it
// will not work when web debugging or using release builds.
test.skip('GlobalEvalWithSourceUrlTest', async () =>
  await loadTestComponent(socket, 'GlobalEvalWithSourceUrlTest'));
