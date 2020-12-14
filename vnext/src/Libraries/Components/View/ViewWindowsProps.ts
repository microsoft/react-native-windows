/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {IKeyboardProps} from '../Keyboard/KeyboardExtProps';
import {ViewProps} from 'react-native';

/**
 * @remarks
 * Props type for ViewWindows component
 *
 * Extends: {@link IKeyboardProps} and {@link https://facebook.github.io/react-native/docs/view | react-native's ViewProps}
 */
export interface IViewWindowsProps extends IKeyboardProps, ViewProps {
  children?: any;

  /**
   * Indicates to accessibility services that the UI Component is within a set and has the given numbered position.
   *
   * See https://github.com/ReactWindows/discussions-and-proposals/blob/harinik-accessibility/proposals/0000-accessibilityapis-lists.md
   */
  accessibilityPosInSet?: number;

  /**
   * Indicates to accessibility services that the UI Component is within a set with the given size.
   *
   * See https://github.com/ReactWindows/discussions-and-proposals/blob/harinik-accessibility/proposals/0000-accessibilityapis-lists.md
   */
  accessibilitySetSize?: number;
  focusable?: boolean;
}
