/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict
 * @format
 */

import type {TurboModule} from '../TurboModule/RCTExport';
import * as TurboModuleRegistry from '../TurboModule/TurboModuleRegistry';
import type {Int32} from '../Types/CodegenTypes';

/* 'buttonClicked' | 'dismissed' */
type DialogAction = string;
/*
  buttonPositive = -1,
  buttonNegative = -2,
  buttonNeutral = -3
*/
type DialogButtonKey = Int32;
export type DialogOptions = {|
  title?: string,
  message?: string,
  buttonPositive?: string,
  buttonNegative?: string,
  buttonNeutral?: string,
  items?: Array<string>,
  cancelable?: boolean,
  defaultButton?: Int32,
|};

export interface Spec extends TurboModule {
  +getConstants: () => {|
    +buttonClicked: DialogAction,
    +dismissed: DialogAction,
    +buttonPositive: DialogButtonKey,
    +buttonNegative: DialogButtonKey,
    +buttonNeutral: DialogButtonKey,
  |};
  +showAlert: (
    config: DialogOptions,
    onError: (error: string) => void,
    onAction: (action: DialogAction, buttonKey?: DialogButtonKey) => void,
  ) => void;
}

export default (TurboModuleRegistry.get<Spec>('Alert'): ?Spec);
