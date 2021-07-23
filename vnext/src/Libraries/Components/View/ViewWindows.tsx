/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {IViewWindowsProps} from './ViewWindowsProps';
import React from 'react';
import {View} from 'react-native';

type ViewWindowsType = React.ForwardRefExoticComponent<
  IViewWindowsProps & React.RefAttributes<View>
> &
  View;

/**
 * Same as {@link https://facebook.github.io/react-native/docs/view | react-native's View}, but with extra Windows specific functionality
 *
 * @remarks
 * Prop type: {@link IViewWindowsProps}.
 *
 */
export const ViewWindows = React.forwardRef(
  (props: IViewWindowsProps, ref: React.Ref<any>) => (
    <View ref={ref} {...props} />
  ),
) as ViewWindowsType;

export type ViewWindows = ViewWindowsType;
