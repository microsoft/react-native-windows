/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */

'use strict';

import type {TurboModule} from 'react-native';
import * as TurboModuleRegistry from 'react-native/Libraries/TurboModule/TurboModuleRegistry';

export interface Spec extends TurboModule {
  // Exported methods.
  +startFromJS: () => void;
}

export default (TurboModuleRegistry.getEnforcing<Spec>(
  'MyTrivialTurboModule',
): Spec);
