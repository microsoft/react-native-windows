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

export class AutomationClient {
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

    // Localhost TCP can coalesce responses into one chunk, so drain every
    // complete frame and advance past it; stopping after one leaves later
    // responses stuck behind it and wedges the channel.
    while (this.receiveBuffer.length >= 4) {
      const messageLength = this.receiveBuffer.readUInt32LE();
      const totalLength = messageLength + 4;
      if (this.receiveBuffer.length < totalLength) {
        break;
      }

      const messageBuffer = this.receiveBuffer.subarray(4, totalLength);
      this.receiveBuffer = this.receiveBuffer.subarray(totalLength);

      // One bad frame can't be tied to a specific request, so fail every
      // in-flight request rather than let any hang forever (invoke has no timeout).
      try {
        this.onMessage(messageBuffer);
      } catch (err) {
        this.failAllPendingRequests(
          new Error(
            'Unexpected error handling automation-channel message: ' +
              (err instanceof Error ? err.message : String(err)),
          ),
        );
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
      this.failAllPendingRequests(
        new Error('Unexpected JSON-RPC batch response'),
      );
      return;
    }

    switch (response.type) {
      // The server only ever sends responses; a request/notification frame means
      // the stream is out of sync, so fail all in-flight requests rather than
      // silently ignore it (invoke has no timeout).
      case RpcStatusType.request:
      case RpcStatusType.notification:
        this.failAllPendingRequests(
          new Error('Unexpected JSON-RPC message from server'),
        );
        return;

      case RpcStatusType.invalid: {
        // jsonrpc-lite keeps the raw response under `data`; use its id to reject
        // the matching request so its caller fails fast instead of hanging.
        const rawId = (response.payload as any)?.data?.id;
        this.rejectPendingRequest(
          rawId,
          new Error(
            'Invalid JSON-RPC2 response: ' +
              JSON.stringify(response.payload, null, 2),
          ),
        );
        return;
      }

      case RpcStatusType.success: {
        const pendingReq = this.pendingRequests.get(response.payload.id);
        if (pendingReq) {
          this.pendingRequests.delete(response.payload.id);
          pendingReq({type: 'success', result: response.payload.result}, null);
        }
        return;
      }

      case RpcStatusType.error: {
        const pendingReq = this.pendingRequests.get(response.payload.id);
        if (pendingReq) {
          this.pendingRequests.delete(response.payload.id);
          pendingReq({type: 'error', ...response.payload.error}, null);
        }
        return;
      }
    }
  }

  private rejectPendingRequest(id: any, err: Error) {
    if (id === undefined) {
      // No id to attribute the failure to; fail every in-flight request so none
      // hang (invoke has no timeout).
      this.failAllPendingRequests(err);
      return;
    }

    const pendingReq = this.pendingRequests.get(id);
    if (pendingReq) {
      this.pendingRequests.delete(id);
      pendingReq(null, err);
    } else {
      // Recoverable id, but already settled (stale/duplicate) — ignore.
      console.error(err.message);
    }
  }

  private failAllPendingRequests(err: Error) {
    const pending = Array.from(this.pendingRequests.values());
    this.pendingRequests.clear();
    for (const req of pending) {
      req(null, err);
    }
  }
}

export function waitForConnection(opts: {
  port: number;
}): Promise<AutomationClient> {
  return new Promise((resolve, reject) => {
    const server = new Server();
    server.listen(opts.port);

    const onError = (err: Error) => reject(err);
    server.on('error', onError);
    server.on('connection', socket => {
      server.off('error', onError);
      resolve(new AutomationClient(socket, server));
    });
  });
}
