/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<2c767c8d245bddb0861562df84545b86>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\ActivityIndicator\ActivityIndicator.js
 */

import type { HostInstance } from "../../../src/private/types/HostInstance";
import type { ViewProps } from "../View/ViewPropTypes";
import { type ColorValue } from "../../StyleSheet/StyleSheet";
import * as React from "react";
export type ActivityIndicatorInstance = HostInstance;
type IndicatorSize = number | "small" | "large";
type ActivityIndicatorIOSProps = Readonly<{
  /**
   * Whether the indicator should hide when not animating.
   *
   * @platform ios
   */
  hidesWhenStopped?: boolean | undefined;
}>;
type ActivityIndicatorPropsCore = Readonly<{
  /**
   * Whether to show the indicator (`true`) or hide it (`false`).
   */
  animating?: boolean | undefined;
  /**
   * The foreground color of the spinner.
   *
   * @default {@platform android} `null` (system accent default color)
   * @default {@platform ios} '#999999'
   */
  color?: ColorValue | undefined;
  /**
   * Size of the indicator.
   *
   * Small has a height of 20, large has a height of 36.
   *
   * @type enum(`'small'`, `'large'`)
   * @type {@platform android} number
   */
  size?: IndicatorSize | undefined;
}>;
export interface ActivityIndicatorProps extends Readonly<Omit<ViewProps, keyof ActivityIndicatorIOSProps | keyof ActivityIndicatorPropsCore | keyof {}> & Omit<ActivityIndicatorIOSProps, keyof ActivityIndicatorPropsCore | keyof {}> & Omit<ActivityIndicatorPropsCore, keyof {}>> {}
declare const ActivityIndicator: (props: Omit<ActivityIndicatorProps, "ref"> & {
  ref?: React.Ref<ActivityIndicatorInstance> | undefined;
}) => React.ReactNode;
/**
 * Displays a circular loading indicator.
 *
 * Example:
 *
 * ```tsx
 * import React from 'react';
 * import {ActivityIndicator, StyleSheet, View} from 'react-native';
 *
 * const App = () => (
 *   <View style={[styles.container, styles.horizontal]}>
 *     <ActivityIndicator />
 *     <ActivityIndicator size="large" />
 *     <ActivityIndicator size="small" color="#0000ff" />
 *     <ActivityIndicator size="large" color="#00ff00" />
 *   </View>
 * );
 *
 * const styles = StyleSheet.create({
 *   container: {
 *     flex: 1,
 *     justifyContent: 'center',
 *   },
 *   horizontal: {
 *     flexDirection: 'row',
 *     justifyContent: 'space-around',
 *     padding: 10,
 *   },
 * });
 *
 * export default App;
 * ```
 *
 * @see https://reactnative.dev/docs/activityindicator
 */
declare const $$ActivityIndicator: typeof ActivityIndicator;
declare type $$ActivityIndicator = typeof $$ActivityIndicator;
export default $$ActivityIndicator;
