// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
'use strict';

import * as React from 'react';
import { requireNativeComponent, StyleSheet } from 'react-native';
import { IPopupProps } from './PopupProps';

const styles = StyleSheet.create({
  rctPopup: {
    position: 'absolute'
  },
});

const RCTPopup = requireNativeComponent('RCTPopup');

/**
 * Renders a popup component.
 *
 * This is a controlled component that requires an `onDismiss` callback that
 * updates the `isOpen` prop in order for the component to reflect user actions.
 *
 * @keyword popup
 */
export class Popup extends React.Component<IPopupProps> {

  public render(): JSX.Element {
    const props = { ...this.props };
    props.style = [styles.rctPopup, this.props.style];

    return (
      <RCTPopup
        { ...props }
      />
    );
  }
}

export default Popup;
