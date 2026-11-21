/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<440fab2c41e9807e7fb034b8fdbcdd60>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Utilities\codegenNativeCommands.js
 */

type NativeCommandsOptions<T = string> = Readonly<{
  supportedCommands: ReadonlyArray<T>;
}>;
declare function codegenNativeCommands<T extends {}>(options: NativeCommandsOptions<keyof T & string>): T;
declare const $$codegenNativeCommands: typeof codegenNativeCommands;
declare type $$codegenNativeCommands = typeof $$codegenNativeCommands;
export default $$codegenNativeCommands;
