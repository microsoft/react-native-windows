// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import * as React from 'react';
import { requireNativeComponent, StyleSheet } from 'react-native';
import { IMenuFlyoutSubItemProps } from './MenuFlyoutSubItemProps';

const styles = StyleSheet.create({
  rctMenuFlyoutSubItem: {
    position: 'absolute',
  },
});

const RCTMenuFlyoutSubItem = requireNativeComponent('RCTMenuFlyoutSubItem');

/**
 * Renders a MenuFlyoutSubItem component.
 * @keyword MenuFlyoutSubItem
 */
export class MenuFlyoutSubItem extends React.Component<IMenuFlyoutSubItemProps> {

  constructor(props: IMenuFlyoutSubItemProps) {
    super(props);
  }

  public render(): JSX.Element {
    const props = { ...this.props };

    return (
      <RCTMenuFlyoutSubItem
        { ...props }
        style={[styles.rctMenuFlyoutSubItem, this.props.style]}
      />
    );
  }
}

export default MenuFlyoutSubItem;
