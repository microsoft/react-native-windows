/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<8fd0c6f87be863d77c34634b78661776>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\geometry\DOMRect.js
 */

import DOMRectReadOnly, { type DOMRectInit } from "./DOMRectReadOnly";
/**
 * A `DOMRect` describes the size and position of a rectangle.
 * The type of box represented by the `DOMRect` is specified by the method or property that returned it.
 *
 * This is a (mostly) spec-compliant version of `DOMRect` (https://developer.mozilla.org/en-US/docs/Web/API/DOMRect).
 */
declare class DOMRect extends DOMRectReadOnly {
  /**
   * The x coordinate of the `DOMRect`'s origin.
   */
  get x(): number;
  set x(x: null | undefined | number);
  /**
   * The y coordinate of the `DOMRect`'s origin.
   */
  get y(): number;
  set y(y: null | undefined | number);
  /**
   * The width of the `DOMRect`.
   */
  get width(): number;
  set width(width: null | undefined | number);
  /**
   * The height of the `DOMRect`.
   */
  get height(): number;
  set height(height: null | undefined | number);
  /**
   * Creates a new `DOMRect` object with a given location and dimensions.
   */
  static fromRect(rect?: null | undefined | DOMRectInit): DOMRect;
}
export default DOMRect;
