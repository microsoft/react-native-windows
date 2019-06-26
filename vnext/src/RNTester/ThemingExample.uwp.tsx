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
    highContrastElementRGBValues: 'None',
    currentTheme: theming.currentTheme
  };

  componentDidMount() {
    theming.addListener('highContrastDidChange', this.highContrastChanged);
    theming.addListener('themeDidChange', this.themeChanged);
  }

  // TODO: Make args props
  highContrastChanged = (args: any) => {
    const isHighContrast = theming.isHighContrast;
    const highContrastElementRGBValues = args.highContrastElementRGBValues;
    this.setState({ isHighContrast, highContrastElementRGBValues });
  };

  themeChanged = (args: any) => {
    const currentTheme = theming.currentTheme;
    this.setState({currentTheme});
  };

  public render() {
    return (
      <View>
        <Text>isHighContrast: {this.state.isHighContrast ? 'true' : 'false'}</Text>
        <Text>highContrastScheme: {this.state.highContrastElementRGBValues}</Text>
        <Text>currentTheme: {this.state.currentTheme}</Text>
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