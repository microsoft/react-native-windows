/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<e05dd49c0acdcfe1970655a67a7b03cc>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Modal\Modal.js
 */

import type { HostInstance } from "../../src/private/types/HostInstance";
import type { ViewProps } from "../Components/View/ViewPropTypes";
import type { DirectEventHandler } from "../Types/CodegenTypes";
import { type ColorValue } from "../StyleSheet/StyleSheet";
import * as React from "react";
export type ModalInstance = HostInstance;
type OrientationChangeEvent = Readonly<{
  orientation: "portrait" | "landscape";
}>;
type ModalBasePropsCore = {
  /**
   * Controls how the modal animates. `'slide'` slides in from the bottom,
   * `'fade'` fades into view, `'none'` appears without animation.
   *
   * @default `'none'`
   */
  animationType?: ("none" | "slide" | "fade") | undefined;
  /**
   * Whether the modal fills the entire view. Setting to `true` renders the
   * modal over a transparent background.
   *
   * @default `false`
   */
  transparent?: boolean | undefined;
  /**
   * Whether the modal is visible.
   *
   * @default `true`
   */
  visible?: boolean | undefined;
  /**
   * Called when the user taps the hardware back button on Android, the menu
   * button on Apple TV, or the modal is dismissed via drag gesture on iOS
   * (when `allowSwipeDismissal` is `true`). Required on Android and TV.
   */
  onRequestClose?: DirectEventHandler<null> | undefined;
  /**
   * Called once the modal has been shown.
   */
  onShow?: DirectEventHandler<null> | undefined;
  /**
   * The backdrop color of the modal's container. Defaults to `white` if
   * `transparent` is `false`. Ignored if `transparent` is `true`.
   */
  backdropColor?: ColorValue | undefined;
  /**
   * A ref to the native Modal component.
   */
  modalRef?: React.Ref<ModalInstance> | undefined;
};
export interface ModalBaseProps extends ModalBasePropsCore {}
export type ModalPropsIOS = {
  /**
   * Controls how the modal appears.
   *
   * @default `'fullScreen'` if `transparent` is `false`, `'overFullScreen'` if `transparent` is `true`.
   *
   * @platform ios
   */
  presentationStyle?: ("fullScreen" | "pageSheet" | "formSheet" | "overFullScreen") | undefined;
  /**
   * Array of orientations the modal can be rotated to. On iOS, the modal is
   * still restricted by what is specified in your app's Info.plist
   * `UISupportedInterfaceOrientations` field.
   *
   * @default `['portrait']`
   *
   * @platform ios
   */
  supportedOrientations?: ReadonlyArray<"portrait" | "portrait-upside-down" | "landscape" | "landscape-left" | "landscape-right"> | undefined;
  /**
   * Called once the modal has been dismissed.
   *
   * @platform ios
   */
  onDismiss?: (() => void) | undefined;
  /**
   * Called when the orientation changes while the modal is displayed. The
   * orientation provided is only `'portrait'` or `'landscape'`. This callback
   * is also called on initial render, regardless of the current orientation.
   *
   * @platform ios
   */
  onOrientationChange?: DirectEventHandler<OrientationChangeEvent> | undefined;
  /**
   * Controls whether the modal can be dismissed by swiping down. Requires
   * `onRequestClose` to be set.
   *
   * @default `false`
   *
   * @platform ios
   */
  allowSwipeDismissal?: boolean | undefined;
};
export type ModalPropsAndroid = {
  /**
   * Controls whether to force hardware acceleration for the underlying window.
   *
   * @default `false`
   *
   * @platform android
   */
  hardwareAccelerated?: boolean | undefined;
  /**
   * Whether the modal should go under the system statusbar.
   *
   * @default `false`
   *
   * @platform android
   */
  statusBarTranslucent?: boolean | undefined;
  /**
   * Whether the modal should go under the system navigation bar.
   * `statusBarTranslucent` also needs to be `true`.
   *
   * @default `false`
   *
   * @platform android
   */
  navigationBarTranslucent?: boolean | undefined;
};
export type ModalProps = Omit<ModalBaseProps, keyof ModalPropsIOS | keyof ModalPropsAndroid | keyof ViewProps | keyof {}> & Omit<ModalPropsIOS, keyof ModalPropsAndroid | keyof ViewProps | keyof {}> & Omit<ModalPropsAndroid, keyof ViewProps | keyof {}> & Omit<ViewProps, keyof {}> & {};
type ModalRefProps = Readonly<{
  ref?: React.Ref<ModalInstance> | undefined;
}>;
declare function Wrapper($$PARAM_0$$: Omit<ModalRefProps, keyof ModalProps | keyof {}> & Omit<ModalProps, keyof {}> & {}): React.ReactNode;
/**
 * A basic way to present content above an enclosing view.
 *
 * @see https://reactnative.dev/docs/modal
 */
declare const $$Modal: typeof Wrapper;
declare type $$Modal = typeof $$Modal;
export default $$Modal;
