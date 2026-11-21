/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @generated SignedSource<<f50e2cd370e901d3aec2f9fe4351da5e>>
 *
 * This file was translated from Flow by scripts/js-api/build-types/index.js.
 * Original file: vnext\Libraries\Components\Switch\Switch.js
 */

import type { HostInstance } from "../../../src/private/types/HostInstance";
import type { ColorValue } from "../../StyleSheet/StyleSheet";
import type { NativeSyntheticEvent } from "../../Types/CoreEventTypes";
import type { ViewProps } from "../View/ViewPropTypes";
import * as React from "react";
export type SwitchInstance = HostInstance;
export type SwitchPropsIOS = {
  /**
   * Background color when the switch is turned on.
   *
   * @deprecated use trackColor instead
   */
  onTintColor?: ColorValue | undefined;
  /**
   * Color of the foreground switch grip.
   *
   * @deprecated use thumbColor instead
   */
  thumbTintColor?: ColorValue | undefined;
  /**
   * Background color when the switch is turned off.
   *
   * @deprecated use trackColor instead
   */
  tintColor?: ColorValue | undefined;
};
type SwitchChangeEventData = Readonly<{
  target: number;
  value: boolean;
}>;
export type SwitchChangeEvent = NativeSyntheticEvent<SwitchChangeEventData>;
type SwitchPropsBase = {
  /**
   * If true, the user won't be able to toggle the switch.
   *
   * @default `false`
   */
  disabled?: boolean | undefined;
  /**
   * The value of the switch. If true the switch will be turned on.
   *
   * @default `false`
   */
  value?: boolean | undefined;
  /**
   * Color of the foreground switch grip. If set on iOS, the switch grip
   * loses its drop shadow.
   */
  thumbColor?: ColorValue | undefined;
  /**
   * Custom colors for the switch track. On iOS, when the switch value is
   * false, the track shrinks into the border.
   */
  trackColor?: Readonly<{
    false?: ColorValue | undefined;
    true?: ColorValue | undefined;
  }> | undefined;
  /**
   * Custom color for the background. Can be seen when the switch value is
   * false or when the switch is disabled (and the switch is translucent).
   *
   * @platform ios
   */
  ios_backgroundColor?: ColorValue | undefined;
  /**
   * Invoked when the user tries to change the value of the switch. Receives
   * the change event as an argument. Use `onValueChange` to receive just
   * the new value instead.
   */
  onChange?: ((event: SwitchChangeEvent) => Promise<void> | void) | undefined;
  /**
   * Invoked when the user tries to change the value of the switch. Receives
   * the new value as an argument. Use `onChange` to receive the event
   * instead.
   */
  onValueChange?: ((value: boolean) => Promise<void> | void) | undefined;
};
export interface SwitchProps extends Readonly<Omit<ViewProps, keyof SwitchPropsIOS | keyof SwitchPropsBase | keyof {}> & Omit<SwitchPropsIOS, keyof SwitchPropsBase | keyof {}> & Omit<SwitchPropsBase, keyof {}>> {}
declare const Switch: (props: Omit<SwitchProps, "ref"> & {
  ref?: React.Ref<SwitchInstance> | undefined;
}) => React.ReactNode;
/**
 * Renders a boolean input.
 *
 * This is a controlled component that requires an `onValueChange` callback
 * that updates the `value` prop in order for the component to reflect user
 * actions. If the `value` prop is not updated, the component will continue to
 * render the supplied `value` prop instead of the expected result of any user
 * actions.
 *
 * Example:
 *
 * ```tsx
 * import React, {useState} from 'react';
 * import {View, Switch, StyleSheet} from 'react-native';
 *
 * const App = () => {
 *   const [isEnabled, setIsEnabled] = useState(false);
 *   const toggleSwitch = () => setIsEnabled(previousState => !previousState);
 *
 *   return (
 *     <View style={styles.container}>
 *       <Switch
 *         trackColor={{false: '#767577', true: '#81b0ff'}}
 *         thumbColor={isEnabled ? '#f5dd4b' : '#f4f3f4'}
 *         ios_backgroundColor="#3e3e3e"
 *         onValueChange={toggleSwitch}
 *         value={isEnabled}
 *       />
 *     </View>
 *   );
 * };
 *
 * const styles = StyleSheet.create({
 *   container: {
 *     flex: 1,
 *     alignItems: 'center',
 *     justifyContent: 'center',
 *   },
 * });
 *
 * export default App;
 * ```
 *
 * @see https://reactnative.dev/docs/switch
 */
declare const $$Switch: typeof Switch;
declare type $$Switch = typeof $$Switch;
export default $$Switch;
