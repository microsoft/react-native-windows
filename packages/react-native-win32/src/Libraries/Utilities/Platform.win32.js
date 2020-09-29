/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

'use strict';

import NativePlatformConstantsWin from './NativePlatformConstantsWin';

export type PlatformSelectSpec<A, N, D> = {
  win32?: A,
  native?: N,
  default?: D,
  ...
};

const Platform = {
  __constants: null,
  OS: 'win32',
  get Version(): number {
    return this.constants.osVersion;
  },
  get constants(): {|
    isTesting: boolean,
    reactNativeVersion: {|
      major: number,
      minor: number,
      patch: number,
      prerelease: ?number,
    |},
    osVersion: number,
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
  get isTV(): boolean {
    return false;
  },
  select: <A, N, D>(spec: PlatformSelectSpec<A, N, D>): A | N | D =>
    'win32' in spec
      ? spec.win32
      : 'native' in spec
      ? spec.native
      : spec.default,
};

module.exports = Platform;
