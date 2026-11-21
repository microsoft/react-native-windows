/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<3323251139c062568f7c7d0a2ca7e1de>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\oldstylecollections\NodeList.js
 */

import type { ArrayLike } from "../../utils/ArrayLikeUtils";
declare class NodeList<T> implements Iterable<T>, ArrayLike<T> {
  /**
   * Use `createNodeList` to create instances of this class.
   *
   * @private This is not defined in the declaration file, so users will not see
   *          the signature of the constructor.
   */
  constructor(elements: ReadonlyArray<T>);
  get length(): number;
  item(index: number): T | null;
  entries(): Iterator<[number, T]>;
  forEach<ThisType>(callbackFn: (value: T, index: number, array: NodeList<T>) => unknown, thisArg?: ThisType): void;
  keys(): Iterator<number>;
  values(): Iterator<T>;
}
export default NodeList;
/**
 * This is an internal method to create instances of `NodeList`,
 * which avoids leaking its constructor to end users.
 * We can do that because the external definition of `NodeList` lives in
 * `NodeList.js.flow`, not here.
 */
export declare function createNodeList<T>(elements: ReadonlyArray<T>): NodeList<T>;
export declare const NodeList_public: typeof NodeList;
export declare type NodeList_public = typeof NodeList_public;
