/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Executes actions, attempting to group by a given key
 */
export default class BatchingQueue<TKey> {
  private readonly keyedQueues: Map<
    TKey,
    Array<() => Promise<void>>
  > = new Map();
  private currentKey?: TKey;

  enqueue<T>(key: TKey, action: () => Promise<T>): Promise<T> {
    return new Promise((resolve, reject) => {
      if (!this.keyedQueues.has(key)) {
        this.keyedQueues.set(key, []);
      }

      this.keyedQueues.get(key).push(async () => {
        try {
          resolve(await action());
        } catch (ex) {
          reject(ex);
        }
      });

      if (!this.currentKey) {
        this.currentKey = key;
        void this.pumpQueue();
      }
    });
  }

  private async pumpQueue(): Promise<void> {
    const currentQueue = this.keyedQueues.get(this.currentKey!);

    while (currentQueue.length > 0) {
      await currentQueue.shift()!();
    }

    this.keyedQueues.delete(this.currentKey!);
    this.currentKey = undefined;

    // If we have more batches, pick the next greedily based on size
    if (this.keyedQueues.size > 0) {
      let nextKey: TKey | undefined;
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
