/**
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 */

import type * as React from 'react';
import {Constructor} from 'react-native/types/private/Utilities';
import {ViewProps} from './ViewPropTypes';
import {HostInstance} from 'react-native/types/public/ReactNativeTypes';

/**
 * The most fundamental component for building UI, View is a container that supports layout with flexbox, style, some touch handling,
 * and accessibility controls, and is designed to be nested inside other views and to have 0 to many children of any type.
 * View maps directly to the native view equivalent on whatever platform React is running on,
 * whether that is a UIView, <div>, android.view, etc.
 */
declare class ViewWin32Component extends React.Component<ViewProps> {}
declare const ViewWin32Base: Constructor<HostInstance> & typeof ViewWin32Component;
export class ViewWin32 extends ViewWin32Base {
  /**
   * Is 3D Touch / Force Touch available (i.e. will touch events include `force`)
   * @platform ios
   */
  static forceTouchAvailable: boolean;
}
