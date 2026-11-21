/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<4d033ffed406b86348cddf7ad015d921>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\geometry\DOMRectReadOnly.js
 */

export interface DOMRectInit {
  x?: number | undefined;
  y?: number | undefined;
  width?: number | undefined;
  height?: number | undefined;
}
/**
 * The `DOMRectReadOnly` interface specifies the standard properties used by `DOMRect` to define a rectangle whose properties are immutable.
 *
 * This is a (mostly) spec-compliant version of `DOMRectReadOnly` (https://developer.mozilla.org/en-US/docs/Web/API/DOMRectReadOnly).
 */
declare class DOMRectReadOnly {
  constructor(x: null | undefined | number, y: null | undefined | number, width: null | undefined | number, height: null | undefined | number);
  /**
   * The x coordinate of the `DOMRectReadOnly`'s origin.
   */
  get x(): number;
  /**
   * The y coordinate of the `DOMRectReadOnly`'s origin.
   */
  get y(): number;
  /**
   * The width of the `DOMRectReadOnly`.
   */
  get width(): number;
  /**
   * The height of the `DOMRectReadOnly`.
   */
  get height(): number;
  /**
   * Returns the top coordinate value of the `DOMRect` (has the same value as `y`, or `y + height` if `height` is negative).
   */
  get top(): number;
  /**
   * Returns the right coordinate value of the `DOMRect` (has the same value as ``x + width`, or `x` if `width` is negative).
   */
  get right(): number;
  /**
   * Returns the bottom coordinate value of the `DOMRect` (has the same value as `y + height`, or `y` if `height` is negative).
   */
  get bottom(): number;
  /**
   * Returns the left coordinate value of the `DOMRect` (has the same value as `x`, or `x + width` if `width` is negative).
   */
  get left(): number;
  toJSON(): {
    x: number;
    y: number;
    width: number;
    height: number;
    top: number;
    left: number;
    bottom: number;
    right: number;
  };
  /**
   * Creates a new `DOMRectReadOnly` object with a given location and dimensions.
   */
  static fromRect(rect?: null | undefined | DOMRectInit): DOMRectReadOnly;
}
export default DOMRectReadOnly;
