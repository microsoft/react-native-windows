/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
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

export default (TurboModuleRegistry.getEnforcing<Spec>(
  'PlatformConstants',
): Spec);
