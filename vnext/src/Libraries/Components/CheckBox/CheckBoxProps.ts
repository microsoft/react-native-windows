/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import {ViewProps} from 'react-native';

export interface ICheckBoxProps extends ViewProps {
  value?: boolean;
  disabled?: boolean;
  onChange?: (event: ICheckBoxChangeEvent) => void;
  onValueChange?: (value: boolean) => void;
  defaultChecked?: boolean;
}

export interface ICheckBoxChangeEvent {
  nativeEvent: {
    value: boolean;
  };
}
