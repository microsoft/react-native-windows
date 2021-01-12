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
    private keyedQueues;
    private currentKey?;
    enqueue<T>(key: TKey, action: () => Promise<T>): Promise<T>;
    private pumpQueue;
}
