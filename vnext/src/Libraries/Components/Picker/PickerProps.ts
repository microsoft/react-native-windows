/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {ViewProps} from 'react-native';

export interface IPickerItemProps extends ViewProps {
  label: string;
  value?: any;
  color?: string;
  testID?: string;
}

export interface IPickerProps extends ViewProps {
  selectedValue?: any;
  enabled?: boolean;
  prompt?: string;
  testID?: string;
  onChange?: (event: IPickerChangeEvent) => void;
  onValueChange?: (value: any, itemIndex: number, text: string) => void;

  // Editable support
  editable?: boolean;
  text?: string;
}

export interface IPickerChangeEvent {
  nativeEvent: {
    value: any;
    itemIndex: number;
    text: string;
  };
}
