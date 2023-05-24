/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import type {TurboModule} from 'react-native/Libraries/TurboModule/RCTExport';

import {TurboModuleRegistry} from 'react-native';

export enum EnumInt {
  IA = 23,
  IB = 42,
}

export enum EnumFloat {
  FA = 1.23,
  FB = 4.56,
}

export enum EnumNone {
  NA,
  NB,
}

export enum EnumStr {
  SA = 's---a',
  SB = 's---b',
}

export type UnionFloat = 1.44 | 2.88 | 5.76;
export type UnionString = 'One' | 'Two' | 'Three';
export type UnionObject = {value: number} | {low: string};

export type ConstantsStruct = {|
  const1: boolean,
  const2: number,
  const3: string,
|};

export type ObjectStruct = {|
  a: number,
  b: string,
  c?: ?string,
|};

export type ValueStruct = {|
  x: number,
  y: string,
  z: ObjectStruct,
|};

export type CustomHostObject = {};

export interface Spec extends TurboModule {
  +getArray: (arg: Array<ObjectStruct | null>) => Array<ObjectStruct | null>;
  +getBool: (arg: boolean) => boolean;
  +getConstants: () => ConstantsStruct;
  +getCustomEnum: (arg: EnumInt) => EnumInt;
  +getCustomHostObject: () => CustomHostObject;
  +consumeCustomHostObject: (customHostObject: CustomHostObject) => string;
  +getNumEnum: (arg: EnumInt) => EnumFloat;
  +getStrEnum: (arg: EnumNone) => EnumStr;
  +getMap: (arg: {[key: string]: ?number}) => {[key: string]: ?number};
  +getNumber: (arg: number) => number;
  +getObject: (arg: ObjectStruct) => ObjectStruct;
  +getSet: (arg: Array<number>) => Array<number>;
  +getString: (arg: string) => string;
  +getUnion: (x: UnionFloat, y: UnionString, z: UnionObject) => string;
  +getValue: (x: number, y: string, z: ObjectStruct) => ValueStruct;
  +getValueWithCallback: (callback: (value: string) => void) => void;
  +getValueWithPromise: (error: boolean) => Promise<string>;
  +getWithWithOptionalArgs: (optionalArg?: boolean) => ?boolean;
  +voidFunc: () => void;
  +emitCustomDeviceEvent: (eventName: string) => void;
  +voidFuncThrows: () => void;
  +getObjectThrows: (arg: ObjectStruct) => ObjectStruct;
  +promiseThrows: () => Promise<void>;
  +voidFuncAssert: () => void;
  +getObjectAssert: (arg: ObjectStruct) => ObjectStruct;
  +promiseAssert: () => Promise<void>;
}

export default (TurboModuleRegistry.get<Spec>(
  'NativeCxxModuleExampleCxx',
): ?Spec);
