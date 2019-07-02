// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import * as React from 'react';
import { requireNativeComponent, StyleSheet } from 'react-native';
import { IMenuFlyoutItemProps } from './MenuFlyoutItemProps';

const styles = StyleSheet.create({
  rctMenuFlyoutItem: {
    position: 'absolute'
  },
});

const RCTMenuFlyoutItem = requireNativeComponent('RCTMenuFlyoutItem');

/**
 * Renders a MenuFlyoutItem component.
 * @keyword MenuFlyoutItem
 */
export class MenuFlyoutItem extends React.Component<IMenuFlyoutItemProps> {

  constructor(props: IMenuFlyoutItemProps) {
    super(props);
  }

  public render(): JSX.Element {
    const props = { ...this.props };

    return (
      <RCTMenuFlyoutItem
        { ...props }
        style={[styles.rctMenuFlyoutItem, this.props.style]}
      />
    );
  }
}

export default MenuFlyoutItem;
