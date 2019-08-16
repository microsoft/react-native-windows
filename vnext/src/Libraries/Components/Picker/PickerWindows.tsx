/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';

import * as React from 'react';
import {IPickerItemProps, IPickerProps} from './PickerProps';

class PickerItem extends React.Component<IPickerItemProps> {
  public render(): JSX.Element | null {
    return null;
  }
}

export class Picker extends React.Component<IPickerProps> {
  public static Item = PickerItem;

  public render(): JSX.Element | null {
    return null;
  }
}

export default Picker;
