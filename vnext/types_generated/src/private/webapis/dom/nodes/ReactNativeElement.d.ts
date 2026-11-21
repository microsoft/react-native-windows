/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<c2fd707c801206cbceb15f370382e28b>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\webapis\dom\nodes\ReactNativeElement.js
 */

import type { ViewConfig } from "../../../../../Libraries/Renderer/shims/ReactNativeTypes";
import type { HostInstance, MeasureInWindowOnSuccessCallback, MeasureLayoutOnSuccessCallback, MeasureOnSuccessCallback } from "../../../types/HostInstance";
import type { InstanceHandle } from "./internals/NodeInternals";
import type ReactNativeDocument from "./ReactNativeDocument";
import ReadOnlyElement from "./ReadOnlyElement";
declare class ReactNativeElement extends ReadOnlyElement {
  protected constructor();
  get offsetHeight(): number;
  get offsetLeft(): number;
  get offsetParent(): ReadOnlyElement | null;
  get offsetTop(): number;
  get offsetWidth(): number;
  /**
   * React Native compatibility methods
   */

  /**
   * Removes focus from an input or view. This is the opposite of `focus()`.
   */
  blur(): void;
  /**
   * Requests focus for the given input or view. The exact behavior triggered
   * will depend on the platform and type of view.
   */
  focus(): void;
  /**
   * Determines the location on screen, width, and height of the given view and
   * returns the values via an async callback. If successful, the callback will
   * be called with the following arguments:
   *
   *  - x
   *  - y
   *  - width
   *  - height
   *  - pageX
   *  - pageY
   *
   * Note that these measurements are not available until after the rendering
   * has been completed in native. If you need the measurements as soon as
   * possible, consider using the [`onLayout`
   * prop](docs/view.html#onlayout) instead.
   */
  measure(callback: MeasureOnSuccessCallback): void;
  /**
   * Determines the location of the given view in the window and returns the
   * values via an async callback. If the React root view is embedded in
   * another native view, this will give you the absolute coordinates. If
   * successful, the callback will be called with the following
   * arguments:
   *
   *  - x
   *  - y
   *  - width
   *  - height
   *
   * Note that these measurements are not available until after the rendering
   * has been completed in native.
   */
  measureInWindow(callback: MeasureInWindowOnSuccessCallback): void;
  /**
   * Like [`measure()`](#measure), but measures the view relative an ancestor,
   * specified as `relativeToNativeComponentRef`. This means that the returned x, y
   * are relative to the origin x, y of the ancestor view.
   * _Can also be called with a relativeNativeNodeHandle but is deprecated._
   */
  measureLayout(relativeToNativeNode: number | HostInstance, onSuccess: MeasureLayoutOnSuccessCallback, onFail?: () => void): void;
  /**
   * This function sends props straight to native. They will not participate in
   * future diff process - this means that if you do not include them in the
   * next render, they will remain active (see [Direct
   * Manipulation](https://reactnative.dev/docs/the-new-architecture/direct-manipulation-new-architecture)).
   */
  setNativeProps(nativeProps: {}): void;
}
export default ReactNativeElement;
export declare const ReactNativeElement_public: typeof ReactNativeElement;
export declare type ReactNativeElement_public = typeof ReactNativeElement_public;
