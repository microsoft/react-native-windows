/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<a7d26ec4790987f22c7af87c55d29e80>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\src\private\specs_DEPRECATED\modules\NativeUIManager.js
 */

import type { RootTag } from "../../../../Libraries/TurboModule/RCTExport";
import type { TurboModule } from "../../../../Libraries/TurboModule/RCTExport";
import type { UnsafeObject } from "../../../../Libraries/Types/CodegenTypes";
/**
 * The config object for a single native view manager. It is consumed
 * dynamically (callers read arbitrary keys such as `Commands`, `Constants`,
 * `NativeProps`, `bubblingEventTypes`, etc.), so it is intentionally left as an
 * open object type. Keep this in sync with the shape produced by native.
 */
export type ViewManagerConfig = UnsafeObject;
/**
 * The constants object returned by `UIManager.getConstants()`. It is consumed
 * dynamically (callers read arbitrary keys such as `ViewManagerNames`,
 * `LazyViewManagersEnabled`, per-view-manager configs, etc.), so it is
 * intentionally left as an open object type.
 */
export type UIManagerConstants = UnsafeObject;
export type NativeMeasureOnSuccessCallback = (x: number, y: number, width: number, height: number, pageX: number, pageY: number) => void;
export type NativeMeasureInWindowOnSuccessCallback = (x: number, y: number, width: number, height: number) => void;
export type NativeMeasureLayoutOnSuccessCallback = (left: number, top: number, width: number, height: number) => void;
export interface Spec extends TurboModule {
  readonly getConstants: () => UIManagerConstants;
  readonly createView: (reactTag: number, viewName: string, rootTag: RootTag, props: UnsafeObject) => void;
  readonly updateView: (reactTag: number, viewName: string, props: UnsafeObject) => void;
  readonly findSubviewIn: (reactTag: number, point: Array<number>, callback: (nativeViewTag: number, left: number, top: number, width: number, height: number) => void) => void;
  /**
   * Used to call a native view method from JavaScript
   *
   * reactTag - Id of react view.
   * commandID - Id of the native method that should be called.
   * commandArgs - Args of the native method that we can pass from JS to native.
   */
  readonly dispatchViewManagerCommand: (reactTag: number, commandID: number, commandArgs?: Array<unknown>) => void;
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
   *
   * @deprecated Use `ref.measure` instead.
   */
  readonly measure: (reactTag: number, callback: NativeMeasureOnSuccessCallback) => void;
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
   *
   * @deprecated Use `ref.measureInWindow` instead.
   */
  readonly measureInWindow: (reactTag: number, callback: NativeMeasureInWindowOnSuccessCallback) => void;
  readonly viewIsDescendantOf: (reactTag: number, ancestorReactTag: number, callback: (result: Array<boolean>) => void) => void;
  /**
   * Like [`measure()`](#measure), but measures the view relative an ancestor,
   * specified as `relativeToNativeNode`. This means that the returned x, y
   * are relative to the origin x, y of the ancestor view.
   *
   * As always, to obtain a native node handle for a component, you can use
   * `React.findNodeHandle(component)`.
   *
   * @deprecated Use `ref.measureLayout` instead.
   */
  readonly measureLayout: (reactTag: number, ancestorReactTag: number, errorCallback: (error: UnsafeObject) => void, callback: NativeMeasureLayoutOnSuccessCallback) => void;
  readonly measureLayoutRelativeToParent: (reactTag: number, errorCallback: (error: UnsafeObject) => void, callback: (left: number, top: number, width: number, height: number) => void) => void;
  readonly setJSResponder: (reactTag: number, blockNativeResponder: boolean) => void;
  readonly clearJSResponder: () => void;
  readonly configureNextLayoutAnimation: (config: UnsafeObject, callback: () => void, errorCallback: (error: UnsafeObject) => void) => void;
  readonly setChildren: (containerTag: number, reactTags: Array<number>) => void;
  readonly manageChildren: (containerTag: number, moveFromIndices: Array<number>, moveToIndices: Array<number>, addChildReactTags: Array<number>, addAtIndices: Array<number>, removeAtIndices: Array<number>) => void;
  readonly getConstantsForViewManager?: ((viewManagerName: string) => ViewManagerConfig | undefined) | undefined;
  readonly getDefaultEventTypes?: (() => ViewManagerConfig) | undefined;
  /**
   * Automatically animates views to their new positions when the
   * next layout happens.
   *
   * A common way to use this API is to call it before calling `setState`.
   *
   * Note that in order to get this to work on **Android** you need to set the following flags via `UIManager`:
   *
   * ```js
   * UIManager.setLayoutAnimationEnabledExperimental && UIManager.setLayoutAnimationEnabledExperimental(true);
   * ```
   */
  readonly setLayoutAnimationEnabledExperimental?: ((enabled: boolean) => void) | undefined;
  readonly sendAccessibilityEvent?: ((reactTag: number, eventType: number) => void) | undefined;
  readonly lazilyLoadView?: ((name: string) => ViewManagerConfig) | undefined;
  readonly focus?: ((reactTag: number) => void) | undefined;
  readonly blur?: ((reactTag: number) => void) | undefined;
}
declare const $$NativeUIManager: Spec;
declare type $$NativeUIManager = typeof $$NativeUIManager;
export default $$NativeUIManager;
