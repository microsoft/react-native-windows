/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 * @flow
 */

'use strict';

// ComboBox component for testing XAML popup positioning bug #15557
// The ComboBox dropdown popup should appear at the correct position after scrolling

import {codegenNativeComponent} from 'react-native';
import type {CodegenTypes, ViewProps} from 'react-native';

type SelectionChangedEvent = Readonly<{
  selectedIndex: CodegenTypes.Int32;
  selectedValue: string;
}>;

export interface ComboBoxProps extends ViewProps {
  selectedIndex?: CodegenTypes.Int32;
  placeholder?: string;
  onSelectionChanged?: CodegenTypes.DirectEventHandler<SelectionChangedEvent>;
}

export default codegenNativeComponent<ComboBoxProps>('ComboBox');
