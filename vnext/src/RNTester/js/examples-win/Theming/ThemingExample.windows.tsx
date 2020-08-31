/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {Text, View, Button} from 'react-native';
import {Appearance} from 'react-native';

class ThemeExample extends React.Component {
  state = {
    currentTheme: Appearance.getColorScheme(),
  };

  componentDidMount() {
    Appearance.addChangeListener(this.onAppThemeChanged);
  }

  componentWillUnmount() {
    Appearance.addChangeListener(this.onAppThemeChanged);
  }

  onAppThemeChanged = (theme: any) => {
    const currentTheme = theme;
    this.setState({currentTheme});
  };

  _onPress = () => {};

  public render() {
    return (
      <View>
        <Text style={{color: 'red'}}>
          currentTheme: {this.state.currentTheme}
        </Text>
        <Button
          onPress={this._onPress}
          title="click me"
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
