/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

import NativePlatformConstantsWin from './NativePlatformConstantsWin';

export type PlatformSelectSpec<T> = {
  windows?: T,
  native?: T,
  default?: T,
  ...
};

const Platform = {
  __constants: null,
  OS: 'windows',
  // $FlowFixMe[unsafe-getters-setters]
  get Version(): number {
    // $FlowFixMe[object-this-reference]
    return this.constants.osVersion;
  },
  // $FlowFixMe[unsafe-getters-setters]
  get constants(): {|
    isTesting: boolean,
    reactNativeVersion: {|
      major: number,
      minor: number,
      patch: number,
      prerelease: ?string,
    |},
    osVersion: number,
  |} {
    // $FlowFixMe[object-this-reference]
    if (this.__constants == null) {
      // $FlowFixMe[object-this-reference]
      this.__constants = NativePlatformConstantsWin.getConstants();
    }
    // $FlowFixMe[object-this-reference]
    return this.__constants;
  },
  // $FlowFixMe[unsafe-getters-setters]
  get isTesting(): boolean {
    if (__DEV__) {
      // $FlowFixMe[object-this-reference]
      return this.constants.isTesting;
    }
    return false;
  },
  // $FlowFixMe[unsafe-getters-setters]
  get isTV(): boolean {
    // $FlowFixMe[object-this-reference]
    return false;
  },
  select: <T>(spec: PlatformSelectSpec<T>): T =>
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
