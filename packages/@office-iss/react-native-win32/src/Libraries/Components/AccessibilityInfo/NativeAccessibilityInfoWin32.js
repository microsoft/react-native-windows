/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 * @flow
 */

import type {TurboModule} from '../../TurboModule/RCTExport';

import * as TurboModuleRegistry from '../../TurboModule/TurboModuleRegistry';

export interface Spec extends TurboModule {
  +isReduceMotionEnabled: (
    onSuccess: (isReduceMotionEnabled: boolean) => void,
  ) => void;
  +isTouchExplorationEnabled: (
    onSuccess: (isScreenReaderEnabled: boolean) => void,
  ) => void;
  +isAccessibilityServiceEnabled?: ?(
    onSuccess: (isAccessibilityServiceEnabled: boolean) => void,
  ) => void;
  +setAccessibilityFocus: (reactTag: number) => void;
  +announceForAccessibility: (announcement: string) => void;
  // [Win32
  +announceForAccessibilityWithOptions?: (
    announcement: string,
    options: {queue?: boolean, nativeID?: string},
  ) => void;
  // Win32]
  +getRecommendedTimeoutMillis?: (
    mSec: number,
    onSuccess: (recommendedTimeoutMillis: number) => void,
  ) => void;
}

export default (TurboModuleRegistry.get<Spec>('AccessibilityInfo'): ?Spec);
