/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<8acf24f1bba511e63feedde206e7d548>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\nodes\ReadOnlyElement.js
 */

import type HTMLCollection from "../oldstylecollections/HTMLCollection";
import DOMRect from "../../geometry/DOMRect";
import ReadOnlyNode from "./ReadOnlyNode";
declare class ReadOnlyElement extends ReadOnlyNode {
  get childElementCount(): number;
  get children(): HTMLCollection<ReadOnlyElement>;
  get clientHeight(): number;
  get clientLeft(): number;
  get clientTop(): number;
  get clientWidth(): number;
  get firstElementChild(): ReadOnlyElement | null;
  get id(): string;
  get lastElementChild(): ReadOnlyElement | null;
  get nextElementSibling(): ReadOnlyElement | null;
  get nodeName(): string;
  get nodeType(): number;
  get nodeValue(): string | null;
  set nodeValue(value: string);
  get previousElementSibling(): ReadOnlyElement | null;
  get scrollHeight(): number;
  get scrollLeft(): number;
  get scrollTop(): number;
  get scrollWidth(): number;
  get tagName(): string;
  get textContent(): string;
  getBoundingClientRect(): DOMRect;
  /**
   * Pointer Capture APIs
   */
  hasPointerCapture(pointerId: number): boolean;
  setPointerCapture(pointerId: number): void;
  releasePointerCapture(pointerId: number): void;
}
export default ReadOnlyElement;
/**
 * The public API for `getBoundingClientRect` always includes transform,
 * so we use this internal version to get the data without transform to
 * implement methods like `offsetWidth` and `offsetHeight`.
 */
export declare function getBoundingClientRect(element: ReadOnlyElement, $$PARAM_1$$: {
  includeTransform: boolean;
}): DOMRect;
export declare const ReadOnlyElement_public: typeof ReadOnlyElement;
export declare type ReadOnlyElement_public = typeof ReadOnlyElement_public;
