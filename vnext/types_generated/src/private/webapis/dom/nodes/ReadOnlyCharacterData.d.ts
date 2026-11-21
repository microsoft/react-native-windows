/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<90e41cdd32370483f0c2d95c41026399>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\nodes\ReadOnlyCharacterData.js
 */

import type ReadOnlyElement from "./ReadOnlyElement";
import ReadOnlyNode from "./ReadOnlyNode";
declare class ReadOnlyCharacterData extends ReadOnlyNode {
  get nextElementSibling(): ReadOnlyElement | null;
  get previousElementSibling(): ReadOnlyElement | null;
  get data(): string;
  get length(): number;
  /**
   * @override
   */
  get textContent(): string;
  /**
   * @override
   */
  get nodeValue(): string;
  substringData(offset: number, count: number): string;
}
export default ReadOnlyCharacterData;
export declare const ReadOnlyCharacterData_public: typeof ReadOnlyCharacterData;
export declare type ReadOnlyCharacterData_public = typeof ReadOnlyCharacterData_public;
