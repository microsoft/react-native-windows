/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

import type {TurboModule} from '../TurboModule/RCTExport';

import * as TurboModuleRegistry from '../TurboModule/TurboModuleRegistry';

export type PlatformConstantsWin32 = {|
  isTesting: boolean,
  isDisableAnimations?: boolean,
  reactNativeVersion: {|
    major: number,
    minor: number,
    patch: number,
    prerelease: ?string | number,
  |},
  osVersion: number,
|};

export interface Spec extends TurboModule {
  +getConstants: () => PlatformConstantsWin32;
}

export default (TurboModuleRegistry.getEnforcing<Spec>(
  'PlatformConstants',
): Spec);
