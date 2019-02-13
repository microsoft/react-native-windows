// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { Text, View } from 'react-native';

class AccessibilityBaseExample extends React.Component {
  public render() {
    return (
      <View>
        <Text>The following has accessibilityLabel and accessibilityHint:</Text>
        <View
          style={{width:50, height:50, backgroundColor:'blue'}}
          accessibilityLabel="A blue box"
          accessibilityHint="A hint for the blue box."
        />
      </View>
    );
  }
}


export const displayName = (_undefined?: string) => {};
export const title = 'Accessibility';
export const description = 'Usage of accessibility properties.';
export const examples = [
  {
    title: 'Label, Hint',
    render: function(): JSX.Element {
      return <AccessibilityBaseExample />;
    },
  }
];
