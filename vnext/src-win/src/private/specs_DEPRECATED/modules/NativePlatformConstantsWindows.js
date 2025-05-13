/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict
 * @format
 */

import type {TurboModule} from '../../../../Libraries/TurboModule/RCTExport';

import * as TurboModuleRegistry from '../../../../Libraries/TurboModule/TurboModuleRegistry';

export type ReactNativeVersionAndroid = {
  major: number,
  minor: number,
  patch: number,
  prerelease: ?string,
};

export type PlatformConstantsWindows = {|
  isTesting: boolean,
  isDisableAnimations?: boolean,
  reactNativeVersion: ReactNativeVersionAndroid,
  reactNativeWindowsVersion: {|
    major: number,
    minor: number,
    patch: number,
  |},
  osVersion: number,
|};

export interface Spec extends TurboModule {
  +getConstants: () => PlatformConstantsWindows;
}

export default (TurboModuleRegistry.getEnforcing<Spec>(
  'PlatformConstants',
): Spec);
