/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {TestBlock} from './TestDescription';
import IntegrationTestRunner from './IntegrationTestRunner';
import TestWebSocketServer from './TestWebSocketServer';

let testRunner: IntegrationTestRunner | undefined;
let websocketServer: TestWebSocketServer | undefined;

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

export function registerTests(blocks: TestBlock[]) {
  blocks.forEach(block => {
    describe(block.name, () => {
      block.tests.forEach(component => {
        if (typeof component === 'string') {
          test(
            component,
            async () => await testRunner!.runTestComponent(component),
          );
        } else {
          test.skip(
            component.skip,
            async () => await testRunner!.runTestComponent(component.skip),
          );
        }
      });
    });
  });
}
