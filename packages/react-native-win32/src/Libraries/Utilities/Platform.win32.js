/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

'use strict';

import NativePlatformConstantsWin from './NativePlatformConstantsWin';

export type PlatformSelectSpec<D, I> = {
  default?: D,
  win32?: I,
};

const Platform = {
  __constants: null,
  OS: 'win32',
  get constants(): {|
    isTesting: boolean,
    reactNativeVersion: {|
      major: number,
      minor: number,
      patch: number,
      prerelease: ?number,
    |},
  |} {
    if (this.__constants == null) {
      this.__constants = NativePlatformConstantsWin.getConstants();
    }
    return this.__constants;
  },
  get isTesting(): boolean {
    if (__DEV__) {
      return this.constants.isTesting;
    }
    return false;
  },
  get isTV() {
    return false;
  },
  select: <D, I>(spec: PlatformSelectSpec<D, I>): D | I =>
    'win32' in spec ? spec.win32 : spec.default,
};

module.exports = Platform;
