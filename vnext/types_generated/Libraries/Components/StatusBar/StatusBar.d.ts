/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<a8e5953f3abebcb381253938f948f948>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\StatusBar\StatusBar.js
 */

import * as React from "react";
/**
 * Status bar style
 */
export type StatusBarStyle = keyof {
  /**
   * Default status bar style (dark for iOS, light for Android)
   */
  default: string;
  /**
   * Automatically picks `light-content` or `dark-content` based on the current
   * color scheme. Updates whenever the color scheme changes.
   */
  auto: string;
  /**
   * Dark background, white texts and icons
   */
  "light-content": string;
  /**
   * Light background, dark texts and icons
   */
  "dark-content": string;
};
/**
 * Status bar animation
 */
export type StatusBarAnimation = keyof {
  /**
   * No animation
   */
  none: string;
  /**
   * Fade animation
   */
  fade: string;
  /**
   * Slide animation
   */
  slide: string;
};
export type StatusBarPropsIOS = Readonly<{
  /**
   * The transition effect when showing and hiding the status bar using the
   * `hidden` prop.
   *
   * @default `'fade'`
   * @platform ios
   */
  showHideTransition?: ("fade" | "slide" | "none") | undefined;
}>;
type StatusBarBaseProps = Readonly<{
  /**
   * If the status bar is hidden.
   *
   * @default `false`
   */
  hidden?: boolean | undefined;
  /**
   * If the transition between status bar property changes should be animated.
   * Supported for `barStyle` and `hidden`.
   *
   * @default `false`
   */
  animated?: boolean | undefined;
  /**
   * Sets the color of the status bar text.
   *
   * @default `'default'`
   */
  barStyle?: ("default" | "auto" | "light-content" | "dark-content") | undefined;
}>;
export type StatusBarProps = Readonly<Omit<StatusBarPropsIOS, keyof StatusBarBaseProps | keyof {}> & Omit<StatusBarBaseProps, keyof {}> & {}>;
type StackProps = {
  barStyle: {
    value: StatusBarStyle;
    animated: boolean;
  } | undefined;
  hidden: {
    value: boolean;
    animated: boolean;
    transition: StatusBarAnimation;
  } | undefined;
};
/**
 * Component to control the app's status bar. The status bar is the zone,
 * typically at the top of the screen, that displays the current time, Wi-Fi and
 * cellular network information, battery level and/or other status icons.
 *
 * Multiple `StatusBar` components can be mounted simultaneously; props merge in
 * mount order.
 *
 * ### Imperative API
 *
 * For cases where using a component is not ideal, there are static methods
 * to manipulate the `StatusBar` display stack. These methods have the same
 * behavior as mounting and unmounting a `StatusBar` component.
 *
 * For example, you can call `StatusBar.pushStackEntry` to update the status bar
 * before launching a third-party native UI component, and then call
 * `StatusBar.popStackEntry` when completed.
 *
 * Example:
 *
 * ```tsx
 * const openThirdPartyBugReporter = async () => {
 *   // The bug reporter has a dark background, so we push a new status bar style.
 *   const stackEntry = StatusBar.pushStackEntry({barStyle: 'light-content'});
 *
 *   // `open` returns a promise that resolves when the UI is dismissed.
 *   await BugReporter.open();
 *
 *   // Don't forget to call `popStackEntry` when you're done.
 *   StatusBar.popStackEntry(stackEntry);
 * };
 * ```
 *
 * There is a legacy imperative API that enables you to manually update the
 * status bar styles. However, the legacy API does not update the internal
 * `StatusBar` display stack, which means that any changes will be overridden
 * whenever a `StatusBar` component is mounted or unmounted.
 *
 * It is strongly advised that you use `pushStackEntry`, `popStackEntry`, or
 * `replaceStackEntry` instead of the static methods beginning with `set`.
 *
 * ### Constants
 *
 * `currentHeight` (Android only) The height of the status bar.
 *
 * @see https://reactnative.dev/docs/statusbar
 */
declare class StatusBar extends React.Component<StatusBarProps> {
  /**
   * The current height of the status bar on the device.
   *
   * @platform android
   */
  static currentHeight: null | undefined | number;
  /**
   * Show or hide the status bar
   * @param hidden Hide the status bar.
   * @param animation Optional animation when
   *    changing the status bar hidden property.
   */
  static setHidden(hidden: boolean, animation?: StatusBarAnimation): void;
  /**
   * Set the status bar style
   * @param style Status bar style to set
   * @param animated Animate the style change.
   */
  static setBarStyle(style: StatusBarStyle, animated?: boolean): void;
  /**
   * Push a StatusBar entry onto the stack.
   * The return value should be passed to `popStackEntry` when complete.
   *
   * @param props Object containing the StatusBar props to use in the stack entry.
   */
  static pushStackEntry(props: StatusBarProps): StackProps;
  /**
   * Pop a StatusBar entry from the stack.
   *
   * @param entry Entry returned from `pushStackEntry`.
   */
  static popStackEntry(entry: StackProps): void;
  /**
   * Replace an existing StatusBar stack entry with new props.
   *
   * @param entry Entry returned from `pushStackEntry` to replace.
   * @param props Object containing the StatusBar props to use in the replacement stack entry.
   */
  static replaceStackEntry(entry: StackProps, props: StatusBarProps): StackProps;
  componentDidMount(): void;
  componentWillUnmount(): void;
  componentDidUpdate(): void;
  render(): React.ReactNode;
}
export type StatusBarInstance = StatusBar;
export default StatusBar;
