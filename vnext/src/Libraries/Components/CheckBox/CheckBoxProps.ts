/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {ViewProps} from 'react-native';

export interface ICheckBoxProps extends ViewProps {
  value?: boolean;
  disabled?: boolean;
  onChange?: (event: ICheckBoxChangeEvent) => void;
  onValueChange?: (value: boolean) => void;
}

export interface ICheckBoxChangeEvent {
  nativeEvent: {
    value: boolean;
  };
}
