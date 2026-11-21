/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<eb1a3d0799c9091a7568ab40fd34e360>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\StyleSheet\private\_TransformStyle.js
 */

import type AnimatedNode from "../../Animated/nodes/AnimatedNode";
type KeysOfUnion<T> = T extends any ? keyof T : never;
type ValueOfUnion<T, K> = T extends any ? K extends keyof T ? T[K] : never : never;
type MergeUnion<T> = { [K in KeysOfUnion<T>]?: ValueOfUnion<T, K> };
type MaximumOneOf<T extends {}> = { [K in keyof T]: { [P in keyof T]?: P extends K ? T[P] : never } }[keyof { [K in keyof T]: { [P in keyof T]?: P extends K ? T[P] : never } }];
export type ____TransformStyle_Internal = Readonly<{
  /**
   * `transform` accepts an array of transformation objects. Each object specifies
   * the property that will be transformed as the key, and the value to use in the
   * transformation. Objects should not be combined. Use a single key/value pair
   * per object.
   *
   * The rotate transformations require a string so that the transform may be
   * expressed in degrees (deg) or radians (rad). For example:
   *
   * `transform([{ rotateX: '45deg' }, { rotateZ: '0.785398rad' }])`
   *
   * The skew transformations require a string so that the transform may be
   * expressed in degrees (deg). For example:
   *
   * `transform([{ skewX: '45deg' }])`
   */
  transform?: ReadonlyArray<Readonly<MaximumOneOf<MergeUnion<{
    readonly perspective: number | AnimatedNode;
  } | {
    readonly rotate: string | AnimatedNode;
  } | {
    readonly rotateX: string | AnimatedNode;
  } | {
    readonly rotateY: string | AnimatedNode;
  } | {
    readonly rotateZ: string | AnimatedNode;
  } | {
    readonly scale: number | AnimatedNode;
  } | {
    readonly scaleX: number | AnimatedNode;
  } | {
    readonly scaleY: number | AnimatedNode;
  } | {
    readonly translateX: number | string | AnimatedNode;
  } | {
    readonly translateY: number | string | AnimatedNode;
  } | {
    readonly translate: [number | string | AnimatedNode, number | string | AnimatedNode] | AnimatedNode;
  } | {
    readonly skewX: string | AnimatedNode;
  } | {
    readonly skewY: string | AnimatedNode;
  } | {
    readonly matrix: ReadonlyArray<number | AnimatedNode> | AnimatedNode;
  }>>>> | string | undefined;
  /**
   * `transformOrigin` accepts an array with 3 elements - each element either being
   * a number, or a string of a number ending with `%`. The last element cannot be
   * a percentage, so must be a number.
   *
   * E.g. transformOrigin: ['30%', '80%', 15]
   *
   * Alternatively accepts a string of the CSS syntax. You must use `%` or `px`.
   *
   * E.g. transformOrigin: '30% 80% 15px'
   */
  transformOrigin?: [string | number, string | number, string | number] | string | undefined;
}>;
