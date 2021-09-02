/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {
  waitForConnection,
  AutomationClient,
} from '@react-native-windows/automation-channel';
import SerialQueue from './SerialQueue';

export type TestCommand =
  | {name: 'GoToComponent'; component: string}
  | {name: 'RunTestComponent'; component: string};

export type TestCommandResponse =
  | {status: 'okay'}
  | {status: 'passed'}
  | {status: 'failed'}
  | {status: 'timeout'}
  | {status: 'error'; message: string}
  | {
      status: 'exception';
      message: string;
      originalMessage: string;
      name: string;
      callstack: CallStackFrame[];
    };

export type CallStackFrame = {
  file: string;
  method: string;
  line: number;
  column: number;
};

export default class IntegrationTestClient {
  private readonly automationClient: AutomationClient;
  private readonly commandQueue: SerialQueue;

  private constructor(automationClient: AutomationClient) {
    this.automationClient = automationClient;
    this.commandQueue = new SerialQueue();
  }

  static async connect(): Promise<IntegrationTestClient> {
    const automationClient = await waitForConnection({port: 8305});
    return new IntegrationTestClient(automationClient);
  }

  sendTestCommand(command: TestCommand): Promise<TestCommandResponse> {
    return this.commandQueue.enqueue(async () => {
      const rpcResult = await this.automationClient.invoke(command.name, {
        component: command.component,
      });

      if (rpcResult.type === 'error') {
        return {status: 'error', message: rpcResult.message};
      }

      return rpcResult.result as TestCommandResponse;
    });
  }

  close(): Promise<void> {
    return this.commandQueue.enqueue(async () => {
      this.automationClient.close();
    });
  }
}
