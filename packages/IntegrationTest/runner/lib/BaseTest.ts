/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {SkipTest} from './SkipTest';
import IntegrationTestRunner from './IntegrationTestRunner';
import TestWebSocketServer from './TestWebSocketServer';

let testRunner: IntegrationTestRunner;
let websocketServer: TestWebSocketServer;

beforeAll(async () => {
  testRunner = await IntegrationTestRunner.initialize('TestInstructions');
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

export function registerTestComponents(components: Array<string | SkipTest>) {
  components.forEach(component => {
    if (typeof component === 'string') {
      test(component, async () => await testRunner.runTestComponent(component));
    } else {
      test.skip(
        component.skip,
        async () => await testRunner.runTestComponent(component.skip),
      );
    }
  });
}
