/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as ora from 'ora';
import * as psList from 'ps-list';

import {execSync} from 'child_process';

import IntegrationTestClient, {
  TestCommandResponse,
} from './IntegrationTestClient';

export default class IntegrationTestRunner {
  private connectionReset: boolean = false;
  private constructor(private readonly testClient: IntegrationTestClient) {}

  static async initialize(
    startingComponent: string,
  ): Promise<IntegrationTestRunner> {
    await ensureLoopbackServerExemption();
    const testClient = await this.connectToHostWithRetry();
    const res = await testClient.sendTestCommand({
      command: 'GoToComponent',
      component: startingComponent,
    });

    if (res.status === 'error') {
      failWithoutContext(`Error going to initial component: "${res.message}"`);
    } else if (res.status !== 'okay') {
      failWithoutContext(
        `Unexpected response going to initial component: ${JSON.stringify(
          res,
        )}`,
      );
    }
    return new IntegrationTestRunner(testClient);
  }

  async runTestComponent(componentName: string): Promise<void> {
    let res: TestCommandResponse;

    try {
      res = await this.testClient.sendTestCommand({
        command: 'RunTestComponent',
        component: componentName,
      });
    } catch (ex) {
      if (this.connectionReset || ex.code === 'ECONNRESET') {
        this.connectionReset = true;
        failWithoutContext(
          'Connection to the test host was reset. This usually means the application has crashed.',
        );
        return;
      } else {
        throw ex;
      }
    }

    switch (res.status) {
      case 'error':
        failWithoutContext(`Error from test host: "${res.message}"`);
        break;

      case 'exception':
        // Jest will special case exceptions named "AssertionError" to be Node
        // AssertionErrors with extra information on them. Rewrite the name so
        // that Jest doesn't try to pull extra information from Chai assertions.
        if (res.name === 'AssertionError') {
          res.name = 'Error';
          res.message = res.message.replace('AssertionError', 'Error');
        }

        const err = new Error(res.originalMessage);
        err.name = res.name;
        err.stack =
          res.message +
          res.callstack
            .map(
              frame =>
                `\n    at ${frame.method}(${prettifyFile(frame.file)}:${
                  frame.line
                }:${frame.column})`,
            )
            .join();
        fail(err);
        break;

      case 'failed':
        failWithoutContext('TestModule.markTestPassed(false) was called');
        break;

      case 'timeout':
        failWithoutContext(
          'The test timed out without seeing an exception or TestModule result',
        );
        break;

      case 'passed':
        // Do nothing
        break;

      default:
        throw new Error(
          `Unexpected response to test command: ${JSON.stringify(res)}`,
        );
    }
  }

  async teardown(): Promise<void> {
    await this.testClient.close();
  }

  private static async connectToHostWithRetry(): Promise<
    IntegrationTestClient
  > {
    const spinner = ora('Waiting for test host to start');

    while (true) {
      try {
        const testClient = await IntegrationTestClient.connect();
        if (spinner.isSpinning) {
          spinner.succeed();
        }

        return testClient;
      } catch (ex) {
        if (ex.code === 'ECONNREFUSED') {
          spinner.start();
          await new Promise(resolve => setTimeout(resolve, 100));
        } else {
          throw ex;
        }
      }
    }
  }
}

async function ensureLoopbackServerExemption(): Promise<void> {
  const processes = await psList();
  if (!processes.some(p => p.name === 'CheckNetIsolation.exe')) {
    execSync(
      'powershell Start-Process CheckNetIsolation.exe -ArgumentList "LoopbackExempt","-is" -Verb runAs',
    );
  }
}

function failWithoutContext(message: string) {
  const err = new Error(message);
  err.stack = '\n';
  fail(err);
}

// "Borrowed" from LogBoxInspectorStackFrame
function prettifyFile(location: string): string {
  const queryIndex = location.indexOf('?');
  return location.substring(
    location.lastIndexOf('/') + 1,
    queryIndex === -1 ? location.length : queryIndex,
  );
}
