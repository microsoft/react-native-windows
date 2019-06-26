// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

import { IViewWindowsProps } from './ViewWindowsProps';
import * as React from 'react';
import { View } from 'react-native';

// tslint:disable-next-line:no-any
export const ViewWindows = React.forwardRef((props: IViewWindowsProps, ref: React.Ref<any>) => (
  <View ref={ref} {...props}/>
));

export default ViewWindows;
