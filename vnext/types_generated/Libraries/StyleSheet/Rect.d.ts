/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<999ff70c6c1a185352ee7bc4f0a247df>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\StyleSheet\Rect.js
 */

export type Rect = Readonly<{
  bottom?: number | undefined;
  left?: number | undefined;
  right?: number | undefined;
  top?: number | undefined;
}>;
export type Insets = Rect;
export type RectOrSize = Rect | number;
export declare function createSquare(size: number): Rect;
export declare function normalizeRect(rectOrSize: null | undefined | RectOrSize): null | undefined | Rect;
