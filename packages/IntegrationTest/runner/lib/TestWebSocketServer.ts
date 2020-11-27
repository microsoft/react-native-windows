/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {Server} from 'ws';

/**
 * A web socket server with behavior expected for the upstream WebSocketTest
 */
export default class TestWebSocketServer {
  private readonly server: Server;

  private constructor(server: Server) {
    this.server = server;
  }

  public static async start(): Promise<TestWebSocketServer> {
    const server = new Server({port: 5555});
    server.on('connection', socket => {
      socket.on('message', message => {
        socket.send(message + '_response');
      });

      socket.send('hello');
    });

    return new Promise((resolve, reject) => {
      server.on('listening', () => resolve(new TestWebSocketServer(server)));
      server.on('error', err => reject(err));
    });
  }

  public close() {
    this.server.close();
  }
}
