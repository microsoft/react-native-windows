/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

import type {
  Platform as PlatformType,
  PlatformSelectSpec,
} from './Platform.flow';

import NativePlatformConstantsWin from './NativePlatformConstantsWin';

const Platform: PlatformType = {
  __constants: null,
  OS: 'win32',
  // $FlowFixMe[unsafe-getters-setters]
  get Version(): number {
    // $FlowFixMe[object-this-reference]
    return this.constants.osVersion;
  },
  // $FlowFixMe[unsafe-getters-setters]
  get constants(): {|
    forceTouchAvailable: boolean,
    interfaceIdiom: string,
    isTesting: boolean,
    isDisableAnimations?: boolean,
    osVersion: number,
    reactNativeVersion: {|
      major: number,
      minor: number,
      patch: number,
      prerelease: ?string,
    |},
    systemName: string,
    isMacCatalyst?: boolean,
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
  get isDisableAnimations(): boolean {
    // $FlowFixMe[object-this-reference]
    return this.constants.isDisableAnimations ?? this.isTesting;
  },
  // $FlowFixMe[unsafe-getters-setters]
  get isTV(): boolean {
    // $FlowFixMe[object-this-reference]
    return false;
  },
  // $FlowFixMe[unsafe-getters-setters]
  get isVision(): boolean {
    return false;
  },
  select: <T>(spec: PlatformSelectSpec<T>): T =>
    'win32' in spec
      ? // $FlowFixMe[incompatible-return]
        spec.win32
      : 'native' in spec
        ? // $FlowFixMe[incompatible-return]
          spec.native
        : // $FlowFixMe[incompatible-return]
          spec.default,
};

module.exports = Platform;
