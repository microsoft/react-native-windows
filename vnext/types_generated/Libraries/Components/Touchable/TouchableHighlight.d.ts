/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<0e3b70de66a3078c30cc0267d5e94f74>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\Touchable\TouchableHighlight.js
 */

import type { HostInstance } from "../../../src/private/types/HostInstance";
import type { ColorValue } from "../../StyleSheet/StyleSheet";
import type { TouchableWithoutFeedbackProps } from "./TouchableWithoutFeedback";
import { type ViewStyleProp } from "../../StyleSheet/StyleSheet";
import * as React from "react";
export type TouchableHighlightInstance = HostInstance;
type AndroidProps = Readonly<{
  nextFocusDown?: number | undefined;
  nextFocusForward?: number | undefined;
  nextFocusLeft?: number | undefined;
  nextFocusRight?: number | undefined;
  nextFocusUp?: number | undefined;
}>;
type TouchableHighlightBaseProps = Readonly<{
  /**
   * Opacity of the wrapped view when touch is active. Requires `underlayColor` to be set.
   *
   * @default `0.85`
   */
  activeOpacity?: number | undefined;
  /**
   * Color of the underlay shown when touch is active.
   */
  underlayColor?: ColorValue | undefined;
  style?: ViewStyleProp | undefined;
  /**
   * Called immediately after the underlay is shown.
   */
  onShowUnderlay?: (() => void) | undefined;
  /**
   * Called immediately after the underlay is hidden.
   */
  onHideUnderlay?: (() => void) | undefined;
  testOnly_pressed?: boolean | undefined;
  hostRef?: React.Ref<TouchableHighlightInstance> | undefined;
}>;
export interface TouchableHighlightProps extends Readonly<Omit<TouchableWithoutFeedbackProps, keyof AndroidProps | keyof TouchableHighlightBaseProps | keyof {}> & Omit<AndroidProps, keyof TouchableHighlightBaseProps | keyof {}> & Omit<TouchableHighlightBaseProps, keyof {}>> {}
declare const TouchableHighlight: (props: Omit<Readonly<Omit<TouchableHighlightProps, "hostRef">>, "ref"> & {
  ref?: React.Ref<TouchableHighlightInstance> | undefined;
}) => React.ReactNode;
/**
 * A wrapper for making views respond properly to touches. On press down, the opacity of the wrapped view is decreased, which allows the underlay color to show through, darkening or tinting the view.
 *
 * The underlay comes from wrapping the child in a new View, which can affect layout.
 *
 * Must have exactly one child (not zero or more than one). If you wish to have several child components, wrap them in a View.
 *
 * If you need more extensive and future-proof touch handling, use `Pressable`.
 *
 * @see https://reactnative.dev/docs/touchablehighlight
 */
declare const $$TouchableHighlight: typeof TouchableHighlight;
declare type $$TouchableHighlight = typeof $$TouchableHighlight;
export default $$TouchableHighlight;
