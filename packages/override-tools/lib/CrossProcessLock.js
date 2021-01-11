/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
import * as net from 'net';
/**
 * CrossProcessLock implements a non-polling cross-process asynchronus barrier.
 *
 * - The lock is acquired by creating a named pipe
 *
 * - If named pipe creation fails due to an existing pipe, the process
 *   subscribes to the pipe, waiting asynchronusly until it dies.
 *
 * - Once the pipe dies, any subscribers will race to acquire the lock (create
 *   the pipe server)
 */
export default class CrossProcessLock {
    /**
     * Create the lock instance. Does not yet allocate any resources.
     *
     * @param lockName unique name of the shared resource. Must be under 256
     * chars and not use special characters.
     */
    constructor(lockName) {
        this.pipeName = `\\\\.\\pipe\\${lockName}`;
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
    tryLock() {
        if (this.server.listening) {
            return Promise.resolve(false);
        }
        return new Promise((resolve, reject) => {
            const onListening = () => {
                this.server.off('error', onError);
                resolve(true);
            };
            const onError = (err) => {
                this.server.off('listening', onListening);
                // @ts-ignore Typings for EventEmmiter only know about generic JS errors
                if (err.code === 'EADDRINUSE') {
                    resolve(false);
                }
                else {
                    reject(err);
                }
            };
            this.server.once('listening', onListening);
            this.server.once('error', onError);
            try {
                this.server.listen({
                    path: this.pipeName,
                    exclusive: true,
                    readableAll: true,
                });
            }
            catch (ex) {
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
    lock() {
        return __awaiter(this, void 0, void 0, function* () {
            if (this.server.listening) {
                return;
            }
            if (!(yield this.tryLock())) {
                yield this.waitUntilPipeDeath();
                return this.lock();
            }
        });
    }
    waitUntilPipeDeath() {
        return new Promise((resolve, reject) => {
            const socket = new net.Socket();
            // Treat some errors as potential pipe death, since it could have died
            // between the time to create a server and attempting to connect to the
            // existing.
            socket.once('error', err => {
                // @ts-ignore Typings for EventEmmiter only know about generic JS errors
                if (err.code === 'ECONNREFUSED' || err.code === 'ECONNRESET') {
                    resolve();
                }
                else {
                    reject(err);
                }
            });
            socket.once('close', () => resolve());
            socket.setTimeout(0);
            socket.connect({ path: this.pipeName });
        });
    }
    /**
     * Release the shared resource
     */
    unlock() {
        if (!this.server.listening) {
            return Promise.resolve();
        }
        return new Promise((resolve, reject) => {
            const onClose = () => {
                this.server.off('error', onError);
                resolve();
            };
            const onError = (err) => {
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
//# sourceMappingURL=CrossProcessLock.js.map