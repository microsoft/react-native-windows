/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<05ae37f89125405d8db75adf833fffc4>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Core\Devtools\symbolicateStackTrace.js
 */

import type { StackFrame } from "../NativeExceptionsManager";
export type CodeFrame = Readonly<{
  content: string;
  location: {
    row: number;
    column: number;
  } | undefined;
  fileName: string;
}>;
export type SymbolicatedStackTrace = Readonly<{
  stack: Array<StackFrame>;
  codeFrame: CodeFrame | undefined;
}>;
declare function symbolicateStackTrace(stack: Array<StackFrame>, extraData?: unknown): Promise<SymbolicatedStackTrace>;
export default symbolicateStackTrace;
