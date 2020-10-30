/**
 * (c) Facebook, Inc. and its affiliates. Confidential and proprietary.
 *
 * @format
 */

import {requireNativeComponent, ViewProps} from 'react-native';
//import React from 'react';
import * as React from 'react';
type Props = ViewProps & {
  focusDirection?: 'horizontal' | 'vertical';
  children?: React.ReactNode;
};

const FocusZoneViewNative = requireNativeComponent(
  'FocusZoneView',
) as React.ComponentClass<Props>;

export default function FocusZoneView({
  focusDirection = 'vertical',
  ...props
}: Props) {
  return <FocusZoneViewNative focusDirection={focusDirection} {...props} />;
}
