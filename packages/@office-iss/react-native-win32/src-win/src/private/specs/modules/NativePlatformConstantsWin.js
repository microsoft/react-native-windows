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

<<<<<<< Upstream
export type PlatformConstantsAndroid = {
  isTesting: boolean,
  isDisableAnimations?: boolean,
  reactNativeVersion: ReactNativeVersionAndroid,
  Version: number,
  Release: string,
  Serial: string,
  Fingerprint: string,
  Model: string,
  ServerHost?: string,
  uiMode: string,
  Brand: string,
  Manufacturer: string,
};
=======
export type PlatformConstantsWin32 = {|
  isTesting: boolean,
  isDisableAnimations?: boolean,
  reactNativeVersion: {|
    major: number,
    minor: number,
    patch: number,
    prerelease: ?string,
  |},
  forceTouchAvailable: boolean,
  osVersion: number,
  systemName: string,
  interfaceIdiom: string,
  isMacCatalyst?: boolean,
|};
>>>>>>> Override

export interface Spec extends TurboModule {
  +getConstants: () => PlatformConstantsWin32;
}

export default (TurboModuleRegistry.getEnforcing<Spec>(
  'PlatformConstants',
): Spec);
