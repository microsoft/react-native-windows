/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow
 * @format
 */

'use strict';

import type {TurboModule} from 'react-native';
import type {UnsafeObject} from 'react-native/Libraries/Types/CodegenTypes';
import * as TurboModuleRegistry from 'react-native/Libraries/TurboModule/TurboModuleRegistry';

export interface Spec extends TurboModule {
  readonly getConstants: () => {|
    const1: boolean,
    const2: number,
    const3: string,
  |};
  readonly logAction: (actionName: string, value: unknown) => void;
  readonly voidFunc: () => void;
  readonly getBool: (arg: boolean) => boolean;
  readonly getNumber: (arg: number) => number;
  readonly getString: (arg: string) => string;
  readonly getArray: (arg: Array<any>) => Array<any>;
  readonly getObject: (arg: UnsafeObject) => UnsafeObject;
  readonly getValue: (x: number, y: string, z: UnsafeObject) => UnsafeObject;
  readonly getValueWithCallback: (callback: (value: string) => void) => void;
  readonly getValueWithPromise: (error: boolean) => Promise<string>;
}

export default TurboModuleRegistry.getEnforcing<Spec>(
  'MySimpleTurboModuleCxx',
) as Spec;
