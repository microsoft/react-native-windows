/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import {IKeyboardProps} from '../Keyboard/KeyboardExtProps';
import {ViewProps} from 'react-native';

export interface IViewWindowsProps extends IKeyboardProps, ViewProps {
  acceptsKeyboardFocus?: boolean;
  // tslint:disable-next-line:no-any
  children?: any;
}
