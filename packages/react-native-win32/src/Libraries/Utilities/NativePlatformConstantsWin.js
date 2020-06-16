/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

'use strict';

import type {TurboModule} from '../TurboModule/RCTExport';
import * as TurboModuleRegistry from '../TurboModule/TurboModuleRegistry';

export interface Spec extends TurboModule {
  +getConstants: () => {|
    isTesting: boolean,
    reactNativeVersion: {|
      major: number,
      minor: number,
      patch: number,
      prerelease: ?number,
    |},
  |};
}

// Hack: This should be getEnforcing, but devmain won't pick up the native
// module until they use the DLL for React Native Windows 0.61
export default (TurboModuleRegistry.get<Spec>('PlatformConstants'): ?Spec);
