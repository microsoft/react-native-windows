/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import {requireNativeComponent, StyleSheet} from 'react-native';
import {IColorPickerProps} from './ColorPickerProps';
import * as React from 'react';

const styles = StyleSheet.create({
  rctColorPicker: {
    height: 150,
    width: 150,
  },
});

const RCTColorPicker = requireNativeComponent('RCTColorPicker');

export class ColorPicker extends React.Component<IColorPickerProps> {
  public static defaultProps: IColorPickerProps = {};

  public constructor(props: IColorPickerProps) {
    super(props);
  }
  public render(): JSX.Element {
    const props = {
      style: [styles.rctColorPicker, this.props.style],
    };

    return <RCTColorPicker {...props} />;
  }
}
