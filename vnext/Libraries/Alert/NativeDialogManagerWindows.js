/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict
 * @format
 */

import type {TurboModule} from '../../TurboModule/RCTExport';
import * as TurboModuleRegistry from '../../TurboModule/TurboModuleRegistry';
import type {Int32} from '../../Types/CodegenTypes';

export type DialogOptions = {|
  title?: string,
  message?: string,
  buttonPositive?: string,
  buttonNegative?: string,
  buttonNeutral?: string,
  cancelable?: boolean,
  defaultButton?: Int32,
|};

export interface Spec extends TurboModule {
  +showAlert: (
    config: DialogOptions,
    callback: (error: string) => void,
  ) => void;
}

export default (TurboModuleRegistry.get<Spec>('Alert'): ?Spec);
