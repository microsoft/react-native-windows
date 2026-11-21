/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<e28cc65127f83b83aba5c2b6e0f95f74>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\ProgressBarAndroid\ProgressBarAndroidTypes.js
 */

import type { ColorValue } from "../../StyleSheet/StyleSheet";
import type { ViewProps } from "../View/ViewPropTypes";
/**
 * Style of the ProgressBar and whether it shows indeterminate progress (e.g. spinner).
 *
 * `indeterminate` can only be false if `styleAttr` is Horizontal, and requires a
 * `progress` value.
 */
/**
 * Style of the ProgressBar and whether it shows indeterminate progress (e.g. spinner).
 *
 * `indeterminate` can only be false if `styleAttr` is Horizontal, and requires a
 * `progress` value.
 */
type DeterminateProgressBarAndroidStyleAttrProp = {
  styleAttr: "Horizontal";
  indeterminate: false;
  /**
   * The progress value (between 0 and 1).
   */
  progress: number;
};
type IndeterminateProgressBarAndroidStyleAttrProp = {
  /**
   * Style of the ProgressBar. One of:
   *     Horizontal
   *     Normal (default)
   *     Small
   *     Large
   *     Inverse
   *     SmallInverse
   *     LargeInverse
   */
  styleAttr: "Horizontal" | "Normal" | "Small" | "Large" | "Inverse" | "SmallInverse" | "LargeInverse";
  /**
   * If the progress bar will show indeterminate progress.
   * Note that this can only be false if styleAttr is Horizontal.
   */
  indeterminate: true;
};
type ProgressBarAndroidBaseProps = Readonly<{
  /**
   * Whether to show the ProgressBar (true, the default) or hide it (false).
   */
  animating?: boolean | undefined;
  /**
   * Color of the progress bar.
   */
  color?: ColorValue | undefined;
  /**
   * Used to locate this view in end-to-end tests.
   */
  testID?: string | undefined;
}>;
export type ProgressBarAndroidProps = Readonly<Omit<ViewProps, keyof ProgressBarAndroidBaseProps | keyof DeterminateProgressBarAndroidStyleAttrProp | keyof {}> & Omit<ProgressBarAndroidBaseProps, keyof DeterminateProgressBarAndroidStyleAttrProp | keyof {}> & Omit<DeterminateProgressBarAndroidStyleAttrProp, keyof {}> & {}> | Readonly<Omit<ViewProps, keyof ProgressBarAndroidBaseProps | keyof IndeterminateProgressBarAndroidStyleAttrProp | keyof {}> & Omit<ProgressBarAndroidBaseProps, keyof IndeterminateProgressBarAndroidStyleAttrProp | keyof {}> & Omit<IndeterminateProgressBarAndroidStyleAttrProp, keyof {}> & {}>;
