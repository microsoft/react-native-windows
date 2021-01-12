"use strict";
/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Executes actions, attempting to group by a given key
 */
class BatchingQueue {
    constructor() {
        this.keyedQueues = new Map();
    }
    enqueue(key, action) {
        return new Promise((resolve, reject) => {
            if (!this.keyedQueues.has(key)) {
                this.keyedQueues.set(key, []);
            }
            this.keyedQueues.get(key).push(async () => {
                try {
                    resolve(await action());
                }
                catch (ex) {
                    reject(ex);
                }
            });
            if (!this.currentKey) {
                this.currentKey = key;
                this.pumpQueue();
            }
        });
    }
    async pumpQueue() {
        const currentQueue = this.keyedQueues.get(this.currentKey);
        while (currentQueue.length > 0) {
            await currentQueue.shift()();
        }
        this.keyedQueues.delete(this.currentKey);
        this.currentKey = undefined;
        // If we have more batches, pick the next greedily based on size
        if (this.keyedQueues.size > 0) {
            let nextKey;
            let maxLength = 0;
            this.keyedQueues.forEach((queue, key) => {
                if (queue.length > maxLength) {
                    maxLength = queue.length;
                    nextKey = key;
                }
            });
            this.currentKey = nextKey;
            return this.pumpQueue();
        }
    }
}
exports.default = BatchingQueue;
//# sourceMappingURL=BatchingQueue.js.map