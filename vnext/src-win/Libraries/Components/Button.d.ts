/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import * as React from 'react';
import {ButtonProps as RNButtonProps} from 'react-native';

export interface ButtonPropsWindows {
  /**
   * When `accessible` is true, the system will try to invoke this function when the user performs accessibility tap gesture.
   * @platform windows
   */
  onAccessibilityTap?: () => void;
}

export interface ButtonProps extends RNButtonProps, ButtonPropsWindows {}

declare const Button: React.ForwardRefExoticComponent<ButtonProps & React.RefAttributes<any>>;
export default Button;