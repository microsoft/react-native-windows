/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

import {type EventSubscription} from '../vendor/emitter/EventEmitter';

class Dimensions {
  static get(dim: string): Object {
    throw new Error(
      'Having a global Dimensions object is too simplistic for Win32, so this API does not work',
    );
  }

  static set(dims: $ReadOnly<{[key: string]: any, ...}>): void {
    throw new Error(
      'Having a global Dimensions object is too simplistic for Win32, so this API does not work',
    );
  }

  static addEventListener(
    type: 'change',
    handler: Function,
  ): EventSubscription {
    throw new Error(
      'Having a global Dimensions object is too simplistic for Win32, so this API does not work',
    );
  }

  /**
   * @deprecated Use `remove` on the EventSubscription from `addEventListener`.
   */
  static removeEventListener(type: 'change', handler: Function) {
    throw new Error(
      'Having a global Dimensions object is too simplistic for Win32, so this API does not work',
    );
  }
}

module.exports = Dimensions;
