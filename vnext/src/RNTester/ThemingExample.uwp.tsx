// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { Text, View } from 'react-native';

class ThemeExample extends React.Component {
  public render() {
    return (
      <View>
        <Text>Make this do something interesting regarding theme:</Text>
        <View
          style={{width:50, height:50, backgroundColor:'blue'}}
        />
        <Text>Maybe make this do something else interesting regarding theme:</Text>
        <View
          style={{ width: 50, height: 50, backgroundColor: 'red' }}
        />
      </View>
    );
  }
}

export const displayName = (_undefined?: string) => {};
export const title = 'Theming';
export const description = 'Usage of theme properties.';
export const examples = [
  {
    title: 'Theme Aware Control',
    render: function(): JSX.Element {
      return <ThemeExample />;
    },
  }
];
