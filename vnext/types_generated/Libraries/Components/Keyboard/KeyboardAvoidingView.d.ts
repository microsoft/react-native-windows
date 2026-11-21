/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<c1ecf87fed8bdc0a8f0b73881abfc218>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\Keyboard\KeyboardAvoidingView.js
 */

import type { ViewStyleProp } from "../../StyleSheet/StyleSheet";
import type { ViewProps } from "../View/ViewPropTypes";
import View from "../View/View";
import * as React from "react";
type KeyboardAvoidingViewPropsCore = Readonly<{
  /**
   * Specify how to react to the presence of the keyboard.
   */
  behavior?: ("height" | "position" | "padding") | undefined;
  /**
   * The style of the content container (View) when `behavior` is 'position'.
   */
  contentContainerStyle?: ViewStyleProp | undefined;
  /**
   * Whether the `KeyboardAvoidingView` is enabled.
   *
   * @default `true`
   */
  enabled?: boolean | undefined;
  /**
   * Distance between the top of the user screen and the React Native view,
   * may be non-zero in some use cases.
   *
   * @default `0`
   */
  keyboardVerticalOffset?: number | undefined;
}>;
export interface KeyboardAvoidingViewProps extends Readonly<Omit<ViewProps, keyof KeyboardAvoidingViewPropsCore | keyof {}> & Omit<KeyboardAvoidingViewPropsCore, keyof {}>> {}
type KeyboardAvoidingViewState = {
  bottom: number;
};
/**
 * Automatically adjusts its height, position, or bottom padding based on the
 * keyboard height to remain visible while the virtual keyboard is displayed.
 *
 * @see https://reactnative.dev/docs/keyboardavoidingview
 */
declare class KeyboardAvoidingView extends React.Component<KeyboardAvoidingViewProps, KeyboardAvoidingViewState> {
  viewRef: {
    current: React.ComponentRef<typeof View> | null;
  };
  constructor(props: KeyboardAvoidingViewProps);
  componentDidUpdate(_: KeyboardAvoidingViewProps, prevState: KeyboardAvoidingViewState): void;
  componentDidMount(): void;
  componentWillUnmount(): void;
  render(): React.ReactNode;
}
export type KeyboardAvoidingViewInstance = KeyboardAvoidingView;
export default KeyboardAvoidingView;
