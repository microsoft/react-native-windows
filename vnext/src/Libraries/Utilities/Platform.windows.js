/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

import NativePlatformConstantsWin from './NativePlatformConstantsWin';

export type PlatformSelectSpec<A, N, D> = {
  windows?: A,
  native?: N,
  default?: D,
  ...
};

const Platform = {
  __constants: null,
  OS: 'windows',
  // $FlowFixMe[unsafe-getters-setters]
  get Version(): number {
    return this.constants.osVersion;
  },
  // $FlowFixMe[unsafe-getters-setters]
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
  // $FlowFixMe[unsafe-getters-setters]
  get isTesting(): boolean {
    if (__DEV__) {
      return this.constants.isTesting;
    }
    return false;
  },
  // $FlowFixMe[unsafe-getters-setters]
  get isTV(): boolean {
    return false;
  },
  select: <A, N, D>(spec: PlatformSelectSpec<A, N, D>): A | N | D =>
    'windows' in spec
      ? // $FlowFixMe[incompatible-return]
        spec.windows
      : 'native' in spec
      ? // $FlowFixMe[incompatible-return]
        spec.native
      : // $FlowFixMe[incompatible-return]
        spec.default,
};

module.exports = Platform;
