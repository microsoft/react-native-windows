/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
'use strict';
import * as React from 'react';

import {IKeyboardProps} from './KeyboardExtProps';

export const supportKeyboard = <P extends object>(
  _Component: React.ComponentType<P>,
) =>
  class SupportKeyboard extends React.Component<P & IKeyboardProps> {
    public render(): JSX.Element | null {
      return null;
    }
  };
