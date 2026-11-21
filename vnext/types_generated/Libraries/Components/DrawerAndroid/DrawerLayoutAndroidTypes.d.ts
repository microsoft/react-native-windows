/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<ca1015f91921fe737130242b47ffceee>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\DrawerAndroid\DrawerLayoutAndroidTypes.js
 */

import type { MeasureInWindowOnSuccessCallback, MeasureLayoutOnSuccessCallback, MeasureOnSuccessCallback } from "../../../src/private/types/HostInstance";
import type { ColorValue } from "../../StyleSheet/StyleSheet";
import type { NativeSyntheticEvent } from "../../Types/CoreEventTypes";
import type { ViewProps } from "../View/ViewPropTypes";
import * as React from "react";
export type DrawerStates = "Idle" | "Dragging" | "Settling";
export type DrawerSlideEvent = NativeSyntheticEvent<Readonly<{
  offset: number;
}>>;
/**
 * DrawerLayoutAndroid is deprecated and will be removed in a future release.
 * Use `react-native-drawer-layout` instead.
 * @see https://reactnavigation.org/docs/drawer-layout/
 * @deprecated
 */
export type DrawerLayoutAndroidProps = Readonly<Omit<ViewProps, "keyboardDismissMode" | "drawerBackgroundColor" | "drawerPosition" | "drawerWidth" | "drawerLockMode" | "onDrawerSlide" | "onDrawerStateChanged" | "onDrawerOpen" | "onDrawerClose" | "renderNavigationView" | "statusBarBackgroundColor"> & {
  /**
   * Whether the keyboard is dismissed in response to a drag.
   *
   * @default `'none'`
   */
  keyboardDismissMode?: ("none" | "on-drag") | undefined;
  /**
   * Background color of the drawer.
   *
   * @default `'white'`
   */
  drawerBackgroundColor?: ColorValue | undefined;
  /**
   * Side of the screen from which the drawer slides in.
   *
   * @default `'left'`
   */
  drawerPosition: ("left" | "right") | undefined;
  /**
   * Width of the drawer view.
   */
  drawerWidth?: number | undefined;
  /**
   * Lock mode of the drawer. `'unlocked'` responds to touch gestures, `'locked-closed'` stays closed, `'locked-open'` stays open.
   *
   * @default `'unlocked'`
   */
  drawerLockMode?: ("unlocked" | "locked-closed" | "locked-open") | undefined;
  /**
   * Called when there is an interaction with the navigation view.
   */
  onDrawerSlide?: ((event: DrawerSlideEvent) => unknown) | undefined;
  /**
   * Called when the drawer state changes. States: idle, dragging, settling.
   */
  onDrawerStateChanged?: ((state: DrawerStates) => unknown) | undefined;
  /**
   * Called when the navigation view has been opened.
   */
  onDrawerOpen?: (() => unknown) | undefined;
  /**
   * Called when the navigation view has been closed.
   */
  onDrawerClose?: (() => unknown) | undefined;
  /**
   * The navigation view that will be rendered to the side of the screen.
   */
  renderNavigationView: () => React.JSX.Element;
  /**
   * Makes the drawer take the entire screen and draws the background of the status bar to allow it to open over the status bar. Only has effect on API 21+.
   */
  statusBarBackgroundColor?: ColorValue | undefined;
}>;
export type DrawerLayoutAndroidState = {
  drawerOpened: boolean;
};
export interface DrawerLayoutAndroidMethods {
  /**
   * Opens the drawer.
   */
  openDrawer(): void;
  /**
   * Closes the drawer.
   */
  closeDrawer(): void;
  /**
   * Native methods
   */
  blur(): void;
  focus(): void;
  measure(callback: MeasureOnSuccessCallback): void;
  measureInWindow(callback: MeasureInWindowOnSuccessCallback): void;
  measureLayout(relativeToNativeNode: number, onSuccess: MeasureLayoutOnSuccessCallback, onFail?: () => void): void;
  setNativeProps(nativeProps: {}): void;
}
export type DrawerLayoutAndroidInstance = DrawerLayoutAndroidMethods;
