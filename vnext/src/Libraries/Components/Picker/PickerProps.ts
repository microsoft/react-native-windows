/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {ViewProps} from 'react-native';

export interface IPickerItemProps extends ViewProps {
  label: string;
  // tslint:disable-next-line:no-any
  value?: any;
  color?: string;
  testID?: string;
}

export interface IPickerProps extends ViewProps {
  // tslint:disable-next-line:no-any
  selectedValue?: any;
  enabled?: boolean;
  prompt?: string;
  testID?: string;
  onChange?: (event: IPickerChangeEvent) => void;
  // tslint:disable-next-line:no-any
  onValueChange?: (value: any, itemIndex: number, text: string) => void;

  // Editable support
  editable?: boolean;
  text?: string;
}

export interface IPickerChangeEvent {
  nativeEvent: {
    // tslint:disable-next-line:no-any
    value: any;
    itemIndex: number;
    text: string;
  };
}
