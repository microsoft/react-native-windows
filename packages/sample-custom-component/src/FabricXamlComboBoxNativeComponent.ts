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
import type {ViewProps} from 'react-native';
import type {
  DirectEventHandler,
  Int32,
} from 'react-native/Libraries/Types/CodegenTypes';

type SelectionChangedEvent = Readonly<{
  selectedIndex: Int32;
  selectedValue: string;
}>;

export interface ComboBoxProps extends ViewProps {
  selectedIndex?: Int32;
  placeholder?: string;
  onSelectionChanged?: DirectEventHandler<SelectionChangedEvent>;
}

export default codegenNativeComponent<ComboBoxProps>('ComboBox');
