/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {Text, View, Button} from 'react-native';
import {AppTheme} from '../index.uwp';

class ThemeExample extends React.Component {
  state = {
    currentTheme: AppTheme.currentTheme,
  };

  componentDidMount() {
    AppTheme.addListener('appThemeChanged', this.onAppThemeChanged);
  }

  componentWillUnmount() {
    AppTheme.removeListener('appThemeChanged', this.onAppThemeChanged);
  }

  onAppThemeChanged = (_event: any) => {
    const currentTheme = AppTheme.currentTheme;
    this.setState({currentTheme});
  };

  _onPress = () => {};

  public render() {
    return (
      <View>
        <Text>
          {
            'This is temporarily not working as the sample app was not usable in a dark theme, so it is currently forced to be Light themed. In order to change the theme to follow the system default, delete RequestedTheme="Light" from App.xaml'
          }
        </Text>
        <Text style={{color: 'red'}}>
          currentTheme: {this.state.currentTheme}
        </Text>
        <Button
          onPress={this._onPress}
          title={this.state.currentTheme}
          color={this.state.currentTheme === 'dark' ? 'grey' : 'orange'}
        />
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
  },
];
