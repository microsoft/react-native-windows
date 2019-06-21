// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { Text, View } from 'react-native';
import { Theming } from '../../src/index.uwp';

// TODO: Figure out how to make Theming.uwp.ts export a new instance
const theming = new Theming();

class ThemeExample extends React.Component {
  state = {
    isHighContrast: theming.isHighContrast,
    highContrastScheme: ''
  };

  componentDidMount() {
    theming.addListener('highContrastDidChange', this.highContrastHandler);
  }

  // TODO: Make args props
  highContrastHandler = (args: any) => {
    const isHighContrast = theming.isHighContrast;
    const highContrastScheme = args.highContrastScheme;
    this.setState({ isHighContrast, highContrastScheme });
  };

  public render() {
    return (
      <View
        style={{width:200, height:50, backgroundColor: 'cyan'}}>
        <Text>isHighContrast: {this.state.isHighContrast ? 'true' : 'false'}</Text>
        <Text>highContrastScheme: {this.state.highContrastScheme}</Text>
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