/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import type {ButtonProps as RNButtonProps} from 'react-native';

export interface ButtonPropsWindows {
  /**
   * When `accessible` is true, the system will try to invoke this function when the user performs accessibility tap gesture.
   * @platform windows
   */
  onAccessibilityTap?: () => void;
}

export type ButtonProps = RNButtonProps & ButtonPropsWindows;

// Re-export Button with extended props for Windows
export {Button as default} from 'react-native';