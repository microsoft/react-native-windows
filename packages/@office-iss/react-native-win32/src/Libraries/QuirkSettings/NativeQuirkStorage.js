/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow strict
 */

import type {TurboModule} from '../TurboModule/RCTExport';
import * as TurboModuleRegistry from '../TurboModule/TurboModuleRegistry';

export interface Spec extends TurboModule {
  +getConstants: () => {|
    enableFocusAwareRealization: boolean,
  |};
}

export default (TurboModuleRegistry.get<Spec>('QuirkStorage'): ?Spec);
