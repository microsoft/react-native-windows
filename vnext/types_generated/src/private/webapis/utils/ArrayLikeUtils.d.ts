/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<a6ffce0667fe39a88bb57df9fb213508>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\utils\ArrayLikeUtils.js
 */

/**
 * This definition is different from the current built-in type `$ArrayLike`
 * provided by Flow, in that this is an interface and that one is an object.
 *
 * The difference is important because, when using objects, Flow thinks
 * a `length` property would be copied over when using the spread operator,
 * which is incorrect.
 */
export interface ArrayLike<T> extends Iterable<T> {
  [indexer: number]: T;
  readonly length: number;
}
export declare function createValueIterator<T>(arrayLike: ArrayLike<T>): IteratorObject<T>;
export declare function createKeyIterator<T>(arrayLike: ArrayLike<T>): IteratorObject<number>;
export declare function createEntriesIterator<T>(arrayLike: ArrayLike<T>): IteratorObject<[number, T]>;
