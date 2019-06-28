// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { Text, TouchableHighlight, View } from 'react-native';
import { AppTheme } from '../../src/index.uwp';
import { IAppThemeChangedEvent } from 'src/Libraries/AppTheme/AppTheme';

var currentBackgroundColor = ['blue', 'red', '#E6E6E6', '#7fff00'];

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
        <Text>The following has accessible and accessibilityLabel:</Text>
        <View
          style={{ width: 50, height: 50, backgroundColor: 'red' }}
          accessible={true}
          accessibilityLabel="A hint for the red box."
        />
      </View>
    );
  }
}

class HighContrastExample extends React.Component {
  state = {
    isHighContrast: AppTheme.isHighContrast,
    highContrastColorValues: AppTheme.currentHighContrastColorValues,
    currentTheme: AppTheme.currentTheme
  };

  componentDidMount() {
    AppTheme.addListener('highContrastChanged', this.onHighContrastChanged);
    AppTheme.addListener('appThemeChanged', this.onAppThemeChanged);
  }

  componenetWillUnmount() {
    AppTheme.removeListener('highContrastChanged', this.onHighContrastChanged);
    AppTheme.removeListener('appThemeChanged', this.onAppThemeChanged);
  } 

  // TODO: Make args props
  onHighContrastChanged = (event: IAppThemeChangedEvent) => {
    this.setState({isHighContrast : AppTheme.isHighContrast, 
                  highContrastColorValues : AppTheme.currentHighContrastColorValues});
  };

  onAppThemeChanged = (event: any) => {
    this.setState({currentTheme : AppTheme.currentTheme});
  }

  public render() {
    return (
      <View>
        <Text>The following has HighContrast Event awareness:</Text>
        <View
          style={{width: 150, height: 50, backgroundColor: currentBackgroundColor[2]}}
          accessibilityLabel="A blue box"
          accessibilityHint="A hint for the blue box.">
        <Text>isHighContrast: {this.state.isHighContrast ? 'true' : 'false'}</Text>
        </View>
        <Text>ButtonFaceColor value: {this.state.highContrastColorValues.ButtonFaceColor}</Text>
        <Text>ButtonTextColor value: {this.state.highContrastColorValues.ButtonTextColor}</Text>
        <Text>GrayTextColor value: {this.state.highContrastColorValues.GrayTextColor}</Text>
        <Text>HighlightColor value: {this.state.highContrastColorValues.HighlightColor}</Text>
        <Text>HighlightTextColor value: {this.state.highContrastColorValues.HighlightTextColor}</Text>
        <Text>HotlightColor value: {this.state.highContrastColorValues.HotlightColor}</Text>
        <Text>WindowColor value: {this.state.highContrastColorValues.WindowColor}</Text>
        <Text>WindowTextColor value: {this.state.highContrastColorValues.WindowTextColor}</Text>
        <View
          style={{ width: 150, height: 50, backgroundColor: this.state.highContrastColorValues.ButtonFaceColor}}
          accessible={true}
          accessibilityLabel="A hint for the red box."
        />
      </View>
    );
  }
}

class TouchableExamples extends React.Component<{}, any> {
  public state = {
    pressedCount: 0,
  };

  public render() {
    return (
      <View>
        <Text>The following TouchableHighlight has accessibilityLabel, accessibilityHint, accessibilityRole, toolip:</Text>
        <TouchableHighlight
          style={{width:50, height:50, backgroundColor:'blue'}}
          accessibilityLabel="A blue box"
          accessibilityHint="A hint for the blue box."
          accessibilityRole="button"
          onPress={this.press}
          {...{ tooltip: "a blue tooltip" }}
        >
          <Text>Blue</Text>
        </TouchableHighlight>
        <Text accessibilityLiveRegion="polite" accessibilityLabel={"Pressed " + this.state.pressedCount + "times"}>Pressed {this.state.pressedCount} times</Text>
      </View>
    );
  }

  private press = () => {
    this.setState({pressedCount: this.state.pressedCount+1});
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
  },
  {
    title: 'Touchables',
    render: function(): JSX.Element {
      return <TouchableExamples />;
    },
  },
  {
    title: 'HighContrast',
    render: function(): JSX.Element {
      return <HighContrastExample />;
    },
  }
];