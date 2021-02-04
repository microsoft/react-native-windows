/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Executes each async function one-by-onem returning the original function
 * result/exception
 */
export default class SerialQueue {
  private readonly actions: Array<() => Promise<void>> = [];

  enqueue<T>(action: () => Promise<T>): Promise<T> {
    return new Promise((resolve, reject) => {
      this.actions.push(async () => {
        try {
          resolve(await action());
        } catch (ex) {
          reject(ex);
        }
      });

      if (this.actions.length === 1) {
        void this.pumpQueue();
      }
    });
  }

  private async pumpQueue() {
    while (this.actions.length !== 0) {
      await this.actions[0]();
      this.actions.shift();
    }
  }
}
