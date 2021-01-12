/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
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
    private pipeName;
    private server;
    private connections;
    /**
     * Create the lock instance. Does not yet allocate any resources.
     *
     * @param lockName unique name of the shared resource. Must be under 256
     * chars and not use special characters.
     */
    constructor(lockName: string);
    /**
     * Try to acquire the shared resource. Returns false if it is already in use
     * without acquiring the resource. Subsequent tryLock calls after obtaining a
     * lock will return false.
     */
    tryLock(): Promise<boolean>;
    /**
     * Acquire the shared resource, waiting for it to be available.
     */
    lock(): Promise<void>;
    private waitUntilPipeDeath;
    /**
     * Release the shared resource
     */
    unlock(): Promise<void>;
}
