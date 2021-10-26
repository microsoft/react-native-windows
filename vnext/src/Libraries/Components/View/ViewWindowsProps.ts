/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {IKeyboardProps} from '../Keyboard/KeyboardExtProps';
import {NativeSyntheticEvent, ViewProps} from 'react-native';

export type INativeMouseEvent = {
  target: number;
  identifier: number;
  pageX: number;
  pageY: number;
  locationX: number;
  locationY: number;
  timestamp: number;
  pointerType: string;
  force: number;
  isLeftButton: boolean;
  isRightButton: boolean;
  isMiddleButton: boolean;
  isBarrelButtonPressed: boolean;
  isHorizontalScrollWheel: boolean;
  isEraser: boolean;
  shiftKey: boolean;
  ctrlKey: boolean;
  altKey: boolean;
};

export type IMouseEvent = NativeSyntheticEvent<INativeMouseEvent>;

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

  /**
   * Specifies the Tooltip for the view
   */
  tooltip?: string;

  /**
   * Indicates the TabIndex to use for this view
   */
  tabIndex?: number;

  /**
   * Specifies if the control should show System focus visuals
   */
  enableFocusRing?: boolean;

  /**
   * Event fired when the mouse leaves the view
   */
  onMouseLeave?: (args: IMouseEvent) => void;

  /**
   * Event fired when the mouse enters the view
   */
  onMouseEnter?: (args: IMouseEvent) => void;
}
