/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

'use strict';

import type {TurboModule} from '../../TurboModule/RCTExport';
import * as TurboModuleRegistry from '../../TurboModule/TurboModuleRegistry';
import type {NativeOrDynamicColorType} from '../../StyleSheet/NativeOrDynamicColorType'; // [Win32]

export interface Spec extends TurboModule {
  +getConstants: () => {|
    +HEIGHT: number,
    +DEFAULT_BACKGROUND_COLOR: number,
  |};
  // [Windows Allow NativeOrDynamicColorType to make Flow happy (this will never be called)
  +setColor: (
    color: number | NativeOrDynamicColorType,
    animated: boolean,
  ) => void;
  // Windows]
  +setTranslucent: (translucent: boolean) => void;

  /**
   *  - statusBarStyles can be:
   *    - 'default'
   *    - 'dark-content'
   */
  +setStyle: (statusBarStyle?: ?string) => void;
  +setHidden: (hidden: boolean) => void;
}

export default (TurboModuleRegistry.getEnforcing<Spec>(
  'StatusBarManager',
): Spec);
