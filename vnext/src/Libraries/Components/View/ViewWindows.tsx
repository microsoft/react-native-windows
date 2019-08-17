/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {IViewWindowsProps} from './ViewWindowsProps';
import * as React from 'react';
import {View, NativeMethodsMixin} from 'react-native';

/**
 * Same as {@link https://facebook.github.io/react-native/docs/view | react-native's View}, but with extra windows specific functionality
 *
 * @remarks
 * Prop type: {@link IViewWindowsProps}.
 *
 */
export const ViewWindows = React.forwardRef(
  (props: IViewWindowsProps, ref: React.Ref<any>) => (
    <View ref={ref} {...props} />
  ),
);
export type ViewWindows = React.ForwardRefExoticComponent<
  IViewWindowsProps & React.RefAttributes<View>
> &
  NativeMethodsMixin;
