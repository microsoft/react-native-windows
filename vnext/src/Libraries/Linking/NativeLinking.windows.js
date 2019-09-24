/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow
 * @format
 */

// TODO - This file is only needed to reset the default NativeModule name to LinkingManager instead of IntentAndroid.
// It looks like this has been fixed in RN 0.61

'use strict';

import type {TurboModule} from 'RCTExport';
import * as TurboModuleRegistry from 'TurboModuleRegistry';

export interface Spec extends TurboModule {
  // Common interface
  +getInitialURL: () => Promise<string>;
  +canOpenURL: (url: string) => Promise<boolean>;
  +openURL: (url: string) => Promise<void>;
  +openSettings: () => Promise<void>;

  // Events
  +addListener: (eventName: string) => void;
  +removeListeners: (count: number) => void;
}

export default TurboModuleRegistry.getEnforcing<Spec>('LinkingManager');
