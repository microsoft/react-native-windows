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
    RGBValues: theming.RGBValues,
    currentTheme: theming.currentTheme
  };

  componentDidMount() {
    theming.addListener('highContrastChanged', this.onHighContrastChanged);
    theming.addListener('appThemeChanged', this.onAppThemeChanged);
  }

  // TODO: Make args props
  onHighContrastChanged = (event: any) => {
    const isHighContrast = theming.isHighContrast;
    const RGBValues = event.RGBValues;
    this.setState({ isHighContrast, RGBValues });
  };

  onAppThemeChanged = (event: any) => {
    const currentTheme = theming.currentTheme;
    this.setState({currentTheme});
  };

  public render() {
    return (
      <View>
        <Text>isHighContrast: {this.state.isHighContrast ? 'true' : 'false'}</Text>
        <Text>RBGValues: {this.state.RGBValues}</Text>
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