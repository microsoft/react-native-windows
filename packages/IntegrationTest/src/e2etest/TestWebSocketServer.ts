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
  private server: Server;

  constructor() {
    this.server = new Server({port: 5555});

    this.server.on('connection', socket => {
      socket.on('message', message => {
        socket.send(message + '_response');
      });

      socket.send('hello');
    });
  }

  public close() {
    this.server.close();
  }
}
