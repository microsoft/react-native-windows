/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

/**
 * Executes actions sequentially, fullfilling the returned promise once complete.
 */
export default class ActionQueue<T> {
  private actions: Array<() => Promise<void>> = [];

  enqueue(action: () => Promise<T>): Promise<T> {
    return new Promise((resolve, reject) => {
      this.actions.push(async () => {
        try {
          resolve(await action());
        } catch (ex) {
          reject(ex);
        }
      });

      if (this.actions.length === 1) {
        this.pumpQueue();
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
