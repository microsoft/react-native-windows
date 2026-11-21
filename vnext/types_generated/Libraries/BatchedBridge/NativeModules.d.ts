/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<319e945c06ef220ea2b9babb83f17095>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\BatchedBridge\NativeModules.js
 */

export type ModuleConfig = [string, null | undefined | {}, null | undefined | ReadonlyArray<string>, null | undefined | ReadonlyArray<number>, null | undefined | ReadonlyArray<number>];
export type MethodType = "async" | "promise" | "sync";
declare let NativeModules: {
  [moduleName: string]: any;
};
/**
 * Native Modules written in ObjectiveC/Swift/Java exposed via the RCTBridge
 * Define lazy getters for each module. These will return the module if already loaded, or load it if not.
 * See https://reactnative.dev/docs/native-modules-ios
 * @example
 * const MyModule = NativeModules.ModuleName
 */
declare const $$NativeModules: typeof NativeModules;
declare type $$NativeModules = typeof $$NativeModules;
export default $$NativeModules;
