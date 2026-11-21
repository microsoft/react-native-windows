/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<f57ec35b5701dfe5dd2b94c16b21fc1b>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\RefreshControl\RefreshControl.js
 */

import type { ColorValue } from "../../StyleSheet/StyleSheet";
import type { ViewProps } from "../View/ViewPropTypes";
import * as React from "react";
export type RefreshControlPropsIOS = Readonly<{
  /**
   * The color of the refresh indicator.
   *
   * @platform ios
   */
  tintColor?: ColorValue | undefined;
  /**
   * The title displayed under the refresh indicator.
   *
   * @platform ios
   */
  title?: string | undefined;
  /**
   * The color of the refresh indicator title.
   *
   * @platform ios
   */
  titleColor?: ColorValue | undefined;
}>;
export type RefreshControlPropsAndroid = Readonly<{
  /**
   * The colors (at least one) that will be used to draw the refresh indicator.
   *
   * @platform android
   */
  colors?: ReadonlyArray<ColorValue> | undefined;
  /**
   * Whether the pull to refresh functionality is enabled.
   *
   * @default `true`
   * @platform android
   */
  enabled?: boolean | undefined;
  /**
   * The background color of the refresh indicator.
   *
   * @platform android
   */
  progressBackgroundColor?: ColorValue | undefined;
  /**
   * Size of the refresh indicator.
   *
   * @platform android
   */
  size?: ("default" | "large") | undefined;
}>;
type RefreshControlBaseProps = Readonly<{
  /**
   * Called when the view starts refreshing.
   */
  onRefresh?: (() => void | Promise<void>) | undefined;
  /**
   * Whether the view should be indicating an active refresh.
   */
  refreshing: boolean;
  /**
   * Progress view top offset.
   *
   * @default `0`
   */
  progressViewOffset?: number | undefined;
}>;
export interface RefreshControlProps extends Readonly<Omit<ViewProps, keyof RefreshControlPropsIOS | keyof RefreshControlPropsAndroid | keyof RefreshControlBaseProps | keyof {}> & Omit<RefreshControlPropsIOS, keyof RefreshControlPropsAndroid | keyof RefreshControlBaseProps | keyof {}> & Omit<RefreshControlPropsAndroid, keyof RefreshControlBaseProps | keyof {}> & Omit<RefreshControlBaseProps, keyof {}>> {}
/**
 * Used inside a `ScrollView` to add pull to refresh functionality. When the
 * `ScrollView` is at `scrollY: 0`, swiping down triggers an `onRefresh` event.
 *
 * **Note:** `refreshing` is a controlled prop, this is why it needs to be set
 * to `true` in the `onRefresh` function otherwise the refresh indicator will
 * stop immediately.
 *
 * @see https://reactnative.dev/docs/refreshcontrol
 */
declare class RefreshControl extends React.Component<RefreshControlProps> {
  componentDidMount(): void;
  componentDidUpdate(prevProps: RefreshControlProps): void;
  render(): React.ReactNode;
}
export type RefreshControlInstance = RefreshControl;
export default RefreshControl;
