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
