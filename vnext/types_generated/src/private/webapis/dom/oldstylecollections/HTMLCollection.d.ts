/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<6da9f467125c7f776a16677d36da8c59>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\oldstylecollections\HTMLCollection.js
 */

import type { ArrayLike } from "../../utils/ArrayLikeUtils";
declare class HTMLCollection<T> implements Iterable<T>, ArrayLike<T> {
  /**
   * Use `createHTMLCollection` to create instances of this class.
   *
   * @private This is not defined in the declaration file, so users will not see
   *          the signature of the constructor.
   */
  constructor(elements: ReadonlyArray<T>);
  get length(): number;
  item(index: number): T | null;
  /**
   * @deprecated Unused in React Native.
   */
  namedItem(name: string): T | null;
}
export default HTMLCollection;
/**
 * This is an internal method to create instances of `HTMLCollection`,
 * which avoids leaking its constructor to end users.
 * We can do that because the external definition of `HTMLCollection` lives in
 * `HTMLCollection.js.flow`, not here.
 */
export declare function createHTMLCollection<T>(elements: ReadonlyArray<T>): HTMLCollection<T>;
export declare const HTMLCollection_public: typeof HTMLCollection;
export declare type HTMLCollection_public = typeof HTMLCollection_public;
