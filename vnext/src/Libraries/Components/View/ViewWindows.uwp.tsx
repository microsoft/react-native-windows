// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import * as React from 'react';
import * as RN from 'react-native';
import { IViewWindowsProps, IViewWindows } from './ViewWindowsProps';

export class ViewWindows extends React.Component<IViewWindowsProps, {}> implements IViewWindows {
  constructor(props: IViewWindowsProps) {
    super(props);
  }

  public render(): JSX.Element {
    return <RN.View {...this.props} />;
  }
}

export default ViewWindows;