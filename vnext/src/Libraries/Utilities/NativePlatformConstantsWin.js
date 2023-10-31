/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

import type {TurboModule} from '../TurboModule/RCTExport';

import * as TurboModuleRegistry from '../TurboModule/TurboModuleRegistry';

export type PlatformConstantsWindows = {|
  isTesting: boolean,
  isDisableAnimations?: boolean,
  reactNativeVersion: {|
    major: number,
    minor: number,
    patch: number,
    prerelease: ?string,
  |},
  forceTouchAvailable: boolean,
  osVersion: string,
  systemName: string,
  interfaceIdiom: string,
  isMacCatalyst?: boolean,
|};

export interface Spec extends TurboModule {
  +getConstants: () => PlatformConstantsWindows;
}

export default (TurboModuleRegistry.getEnforcing<Spec>(
  'PlatformConstants',
): Spec);
