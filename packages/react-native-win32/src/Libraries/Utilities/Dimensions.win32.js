/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */
'use strict';

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

  static addEventListener(type: 'change', handler: Function) {
    throw new Error(
      'Having a global Dimensions object is too simplistic for Win32, so this API does not work',
    );
  }

  static removeEventListener(type: 'change', handler: Function) {
    throw new Error(
      'Having a global Dimensions object is too simplistic for Win32, so this API does not work',
    );
  }
}

module.exports = Dimensions;
