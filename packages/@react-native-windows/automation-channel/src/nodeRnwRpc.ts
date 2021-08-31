/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Server, Socket} from 'net';
import jsonrpc from 'jsonrpc-lite';
import {RpcStatusType} from 'jsonrpc-lite';

let incrementingId = 0;

export type InvokeResult =
  | {type: 'error'; code: any; message: string}
  | {type: 'success'; result?: any};

export class RpcClient {
  private readonly socket: Socket;
  private readonly server: Server;
  private readonly pendingRequests: Map<
    any,
    (result: InvokeResult | null, err: Error | null) => void
  >;
  private receiveBuffer: Buffer;

  constructor(socket: Socket, server: Server) {
    this.socket = socket;
    this.server = server;

    this.pendingRequests = new Map();
    this.receiveBuffer = Buffer.alloc(0);

    this.socket.on('data', this.onData.bind(this));
    this.socket.on('end', this.onEnd.bind(this));
    this.socket.on('error', this.onError.bind(this));
  }

  invoke(
    methodName: string,
    params: any[] | Record<string, any>,
  ): Promise<InvokeResult> {
    return new Promise((resolve, reject) => {
      const messageId = ++incrementingId;
      this.pendingRequests.set(messageId, (result, err) => {
        if (err) {
          reject(err);
        }
        if (result) {
          resolve(result);
        }
      });

      const requestString = jsonrpc
        .request(messageId, methodName, params)
        .serialize();

      const requestBuffer = Buffer.from(requestString, 'utf-8');
      const sizeBuffer = Buffer.alloc(4);
      sizeBuffer.writeUInt32LE(requestBuffer.length);

      this.socket.write(sizeBuffer);
      this.socket.write(requestBuffer);
    });
  }

  close() {
    this.socket.destroy();
    this.server.close();
  }

  private onData(chunk: Buffer) {
    this.receiveBuffer = Buffer.concat([this.receiveBuffer, chunk]);
    if (this.receiveBuffer.length >= 4) {
      const messageLength = this.receiveBuffer.readUInt32LE();
      const totalLength = messageLength + 4;

      if (this.receiveBuffer.length >= totalLength) {
        const messageBuffer = this.receiveBuffer.slice(4, totalLength);

        if (totalLength < this.receiveBuffer.length) {
          this.receiveBuffer = Buffer.from(this.receiveBuffer, totalLength);
        } else {
          this.receiveBuffer = Buffer.alloc(0);
        }

        this.onMessage(messageBuffer);
      }
    }
  }

  private onEnd() {
    this.pendingRequests.forEach(req =>
      req(null, new Error('Unexpected disconnect from RPC server')),
    );
  }

  private onError(error: Error) {
    this.pendingRequests.forEach(req => req(null, error));
  }

  private onMessage(message: Buffer) {
    const response = jsonrpc.parseJsonRpcString(message.toString('utf8'));
    if (Array.isArray(response)) {
      throw new Error('Expected single message');
    }

    switch (response.type) {
      case RpcStatusType.request:
        throw new Error('Recieved JSON-RPC request instead of response');
      case RpcStatusType.notification:
        throw new Error('Unexpected JSON-RPC notification');
      case RpcStatusType.invalid:
        throw new Error(
          'Invalid JSON-RPC2 response: ' +
            JSON.stringify(response.payload, null, 2),
        );

      case RpcStatusType.success: {
        const pendingReq = this.pendingRequests.get(response.payload.id);
        if (!pendingReq) {
          throw new Error('Could not find pending request from response ID');
        }

        this.pendingRequests.delete(response.payload.id);
        pendingReq({type: 'success', result: response.payload.result}, null);
        break;
      }

      case RpcStatusType.error: {
        const pendingReq = this.pendingRequests.get(response.payload.id);
        if (!pendingReq) {
          throw new Error('Could not find pending request from response ID');
        }

        this.pendingRequests.delete(response.payload.id);
        pendingReq({type: 'error', ...response.payload.error}, null);
        break;
      }
    }
  }
}

export function waitForConnection(opts: {port: number}): Promise<RpcClient> {
  return new Promise((resolve, reject) => {
    const server = new Server();
    server.listen(opts.port);

    const onError = (err: Error) => reject(err);
    server.on('error', onError);
    server.on('connection', socket => {
      server.off('error', onError);
      resolve(new RpcClient(socket, server));
    });
  });
}
