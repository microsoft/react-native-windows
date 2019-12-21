/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.

 * @format
 * @flow
 */
'use strict';

class Dimensions {
  public static get(dim: string): Object {
    throw new Error(
      'Having a global Dimensions object is too simplistic for Win32, so this API does not work',
    );
  }
  public static addEventListener(type: string, handler: Function) {
    throw new Error(
      'Having a global Dimensions object is too simplistic for Win32, so this API does not work',
    );
  }

  public static removeEventListener(type: string, handler: Function) {
    throw new Error(
      'Having a global Dimensions object is too simplistic for Win32, so this API does not work',
    );
  }
}

module.exports = Dimensions;
