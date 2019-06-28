// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { Text, View } from 'react-native';
import { AppTheme } from '../../src/index.uwp';

class ThemeExample extends React.Component {
  state = {
    isHighContrast: AppTheme.isHighContrast,
    RGBValues: AppTheme.currentRGBValues,
    currentTheme: AppTheme.currentTheme
  };

  componentDidMount() {
    AppTheme.addListener('highContrastChanged', this.onHighContrastChanged);
    AppTheme.addListener('appThemeChanged', this.onAppThemeChanged);
  }

  // TODO: Make args props
  onHighContrastChanged = (event: any) => {
    const isHighContrast = AppTheme.isHighContrast;
    const RGBValues = event.RGBValues;
    this.setState({ isHighContrast, RGBValues });
  };

  onAppThemeChanged = (event: any) => {
    const currentTheme = AppTheme.currentTheme;
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
export const title = 'AppTheme';
export const description = 'Usage of theme properties.';
export const examples = [
  {
    title: 'Theme Aware Control',
    render: function(): JSX.Element {
      return <ThemeExample />;
    },
  }
];