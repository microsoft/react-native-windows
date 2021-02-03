/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import SerialQueue from './SerialQueue';
import {Socket} from 'net';

export type TestCommand =
  | {command: 'GoToComponent'; component: string}
  | {command: 'RunTestComponent'; component: string};

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
  private readonly socket: Socket;
  private readonly commandQueue: SerialQueue;

  private constructor(socket: Socket) {
    this.socket = socket;
    this.commandQueue = new SerialQueue();
  }

  static async connect(): Promise<IntegrationTestClient> {
    const socket = await this.connectToHarness();
    return new IntegrationTestClient(socket);
  }

  sendTestCommand(command: TestCommand): Promise<TestCommandResponse> {
    return this.commandQueue.enqueue(() =>
      this.sendTestCommandInQueue(command),
    );
  }

  close(): Promise<void> {
    return this.commandQueue.enqueue(async () => {
      this.socket.destroy();
    });
  }

  private sendTestCommandInQueue(
    command: TestCommand,
  ): Promise<TestCommandResponse> {
    return new Promise((resolve, reject) => {
      let receiveBuffer = Buffer.alloc(0);

      const onError = (err: Error) => {
        this.socket.off('data', onData);
        reject(err);
      };

      const onData = (chunk: Buffer) => {
        receiveBuffer = Buffer.concat([receiveBuffer, chunk]);
        if (receiveBuffer.length >= 4) {
          const messageLength = receiveBuffer.readUInt32LE();
          const totalLength = messageLength + 4;

          if (receiveBuffer.length >= totalLength) {
            const res = receiveBuffer.toString('utf8', 4);
            resolve(JSON.parse(res));

            this.socket.off('data', onData);
            this.socket.off('error', onError);
          }
        }
      };

      this.socket.on('data', onData);
      this.socket.once('error', onError);

      const payload = Buffer.from(JSON.stringify(command), 'utf8');
      const sizeBuffer = Buffer.alloc(4);
      sizeBuffer.writeUInt32LE(payload.length);

      this.socket.write(sizeBuffer);
      this.socket.write(payload);
    });
  }

  private static async connectToHarness(): Promise<Socket> {
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
}
