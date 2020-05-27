/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
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
      // Hack: We shouldn't need to null-check NativePlatformContants, but
      // needed to remove the invariant it is non-null since react-native-win32
      // hasn't picked up the changes we've made in 0.61 to add the module yet.
      // This can be removed when we fix win32 NativePlatformConstantsWin.
      if (NativePlatformConstantsWin) {
        this.__constants = NativePlatformConstantsWin.getConstants();
      } else {
        this.__constants = {
          isTesting: false,
          reactNativeVersion: {
            major: 0,
            minor: 62,
            patch: 0,
            prerelease: undefined,
          },
        };
      }
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
