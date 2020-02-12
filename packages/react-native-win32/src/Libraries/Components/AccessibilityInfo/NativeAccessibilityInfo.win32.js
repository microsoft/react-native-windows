/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */

'use strict';

import type {TurboModule} from '../../TurboModule/RCTExport';
import * as TurboModuleRegistry from '../../TurboModule/TurboModuleRegistry';

export interface Spec extends TurboModule {
  +isReduceMotionEnabled: (
    onSuccess: (isReduceMotionEnabled: boolean) => void,
  ) => void;
  // [Windows
  +isScreenReaderEnabled: (
    onSuccess: (isScreenReaderEnabled: boolean) => void,
  ) => void;
  // Windows]
  
  +setAccessibilityFocus: (reactTag: number) => void;

  // [Windows
  +announceForAccessibility: (announcement: string, reactTag?: ?number) => void;
  // Windows]
}

export default (TurboModuleRegistry.get<Spec>('AccessibilityInfo'): ?Spec);
