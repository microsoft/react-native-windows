/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<d39746b2d032dd107a12e0084ca0ea8a>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\nodes\ReadOnlyText.js
 */

import ReadOnlyCharacterData from "./ReadOnlyCharacterData";
declare class ReadOnlyText extends ReadOnlyCharacterData {
  /**
   * @override
   */
  get nodeName(): string;
  /**
   * @override
   */
  get nodeType(): number;
}
export default ReadOnlyText;
export declare const ReadOnlyText_public: typeof ReadOnlyText;
export declare type ReadOnlyText_public = typeof ReadOnlyText_public;
