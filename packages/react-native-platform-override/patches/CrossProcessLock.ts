/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as net from 'net';
import * as path from 'path';
import {platform, tmpdir} from 'os';

/**
 * CrossProcessLock implements a non-polling cross-process asynchronous barrier.
 *
 * - The lock is acquired by creating a named pipe
 *
 * - If named pipe creation fails due to an existing pipe, the process
 *   subscribes to the pipe, waiting asynchronously until it dies.
 *
 * - Once the pipe dies, any subscribers will race to acquire the lock (create
 *   the pipe server)
 */
export default class CrossProcessLock {
  private readonly ipcPath: string;
  private readonly server: net.Server;
  private readonly connections: Set<net.Socket>;

  /**
   * Create the lock instance. Does not yet allocate any resources.
   *
   * @param lockName unique name of the shared resource. Must be under 256
   * chars and not use special characters.
   */
  constructor(lockName: string) {
    if (platform() === 'win32') {
      this.ipcPath = `\\\\.\\pipe\\${lockName}`;
    } else {
      this.ipcPath = path.join(tmpdir(), `${lockName}`);
    }

    this.server = net.createServer();

    this.connections = new Set();
    this.server.on('connection', conn => {
      this.connections.add(conn);
      conn.on('close', () => this.connections.delete(conn));
    });
  }

  /**
   * Try to acquire the shared resource. Returns false if it is already in use
   * without acquiring the resource. Subsequent tryLock calls after obtaining a
   * lock will return false.
   */
  tryLock(): Promise<boolean> {
    if (this.server.listening) {
      return Promise.resolve(false);
    }

    return new Promise((resolve, reject) => {
      const onListening = () => {
        this.server.off('error', onError);
        resolve(true);
      };

      const onError = (err: Error) => {
        this.server.off('listening', onListening);
        // @ts-ignore Typings for EventEmitter only know about generic JS errors
        if (err.code === 'EADDRINUSE') {
          resolve(false);
        } else {
          reject(err);
        }
      };

      this.server.once('listening', onListening);
      this.server.once('error', onError);

      try {
        this.server.listen({
          path: this.ipcPath,
          exclusive: true,
          readableAll: true,
        });
      } catch (ex) {
        // Old versions of Node will throw an extra TypeError when address is
        // in use that we need to ignore https://github.com/nodejs/node/issues/23917
        if (ex.name !== 'TypeError') {
          throw ex;
        }
      }
    });
  }

  /**
   * Acquire the shared resource, waiting for it to be available.
   */
  async lock(): Promise<void> {
    if (this.server.listening) {
      return;
    }

    if (!(await this.tryLock())) {
      await this.waitUntilPipeDeath();
      return this.lock();
    }
  }

  private waitUntilPipeDeath(): Promise<void> {
    return new Promise((resolve, reject) => {
      const socket = new net.Socket();

      // Treat some errors as potential pipe death, since it could have died
      // between the time to create a server and attempting to connect to the
      // existing.
      socket.once('error', err => {
        // @ts-ignore Typings for EventEmitter only know about generic JS errors
        if (err.code === 'ECONNREFUSED' || err.code === 'ECONNRESET') {
          resolve();
        } else {
          reject(err);
        }
      });

      socket.once('close', () => resolve());

      socket.setTimeout(0);
      socket.connect({path: this.ipcPath});
    });
  }

  /**
   * Release the shared resource
   */
  unlock(): Promise<void> {
    if (!this.server.listening) {
      return Promise.resolve();
    }

    return new Promise((resolve, reject) => {
      const onClose = () => {
        this.server.off('error', onError);
        resolve();
      };
      const onError = (err: Error) => {
        this.server.off('close', onClose);
        reject(err);
      };

      this.server.once('close', onClose);
      this.server.once('error', onError);

      // Closing will only prevent new connections, instead of killing old
      // ones. Manually destroy all existing connections to let them retry the
      // lock.
      this.server.close();
      this.connections.forEach(conn => conn.destroy());
    });
  }
}
