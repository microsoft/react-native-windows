/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import IntegrationTestRunner from './IntegrationTestRunner';
import TestWebSocketServer from './TestWebSocketServer';

let testRunner: IntegrationTestRunner;
let websocketServer: TestWebSocketServer;

beforeAll(async () => {
  testRunner = await IntegrationTestRunner.initialize('IntegrationTestsApp');
  websocketServer = await TestWebSocketServer.start();
});

afterAll(async () => {
  if (testRunner) {
    await testRunner.teardown();
  }
  if (websocketServer) {
    websocketServer.close();
  }
});

test.each([
  'IntegrationTestHarnessTest',
  'AppEventsTest',
  'PromiseTest',
  'WebSocketTest',
])('%s', async component => {
  await testRunner.runTestComponent(component);
});

test.skip.each([
  // This one is seemingly broken upstream since
  // https://github.com/facebook/react-native/commit/61346d303a44b643b6029835018518d48c285a69
  'TimersTest',

  // Our builtin implementation doens't implement mergeItems
  'AsyncStorageTest',

  // ref.measure returns bad results for us (#5934)
  'LayoutEventsTest',

  // Native snapshot checks not implemented
  'SimpleSnapshotTest',
  'ImageSnapshotTest',

  // Image cache control is currently iOS only
  'ImageCachePolicyTest',

  // Skip SyncMethodTest because we don't implement RNTesterTestModule and want
  // to allow web debugging
  'SyncMethodTest',

  // We implement the Android AccessibilityInfo instead of AccessibilityManager
  'AccessibilityManagerTest',

  // globalEvalWithSourceUrl is only set when using JSIExecutor in debug. I.e. it
  // will not work when web debugging
  'GlobalEvalWithSourceUrlTest',
])('%s', async component => await testRunner.runTestComponent(component));
