// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { FlatList, Text, TouchableHighlight, View, StyleSheet } from 'react-native';
import { AppTheme } from '../../src/index.uwp';
import { IAppThemeChangedEvent } from 'src/Libraries/AppTheme/AppThemeTypes';

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
    highContrastColorValues: AppTheme.currentHighContrastColors,
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
                  highContrastColorValues : AppTheme.currentHighContrastColors});
  };

  onAppThemeChanged = (event: any) => {
    this.setState({currentTheme : AppTheme.currentTheme});
  }

  public render() {
    return (
      <View>
        <Text>The following has HighContrast Event awareness:</Text>
        <View>
          <Text>isHighContrast: {this.state.isHighContrast ? 'True' : 'False'}</Text>
        </View>
        <View style = {this.state.isHighContrast? [this.styles.enabled, {backgroundColor: this.state.highContrastColorValues.ButtonFaceColor}] : this.styles.disabled}>
          <Text>ButtonFace High Contrast Hex Value: {this.state.highContrastColorValues.ButtonFaceColor}</Text>
        </View>
        <View style = {this.state.isHighContrast? [this.styles.enabled, {backgroundColor: this.state.highContrastColorValues.ButtonTextColor}] : this.styles.disabled}>
          <Text>ButtonText High Contrast Color Hex Value: {this.state.highContrastColorValues.ButtonTextColor}</Text>
        </View>
        <View style = {this.state.isHighContrast? [this.styles.enabled, {backgroundColor: this.state.highContrastColorValues.GrayTextColor}] : this.styles.disabled}>
          <Text>GrayText High Contrast Color Hex Value: {this.state.highContrastColorValues.GrayTextColor}</Text>
        </View>
        <View style = {this.state.isHighContrast? [this.styles.enabled, {backgroundColor: this.state.highContrastColorValues.HighlightColor}] : this.styles.disabled}> 
          <Text>Highlight High Contrast Color Hex Value: {this.state.highContrastColorValues.HighlightColor}</Text>
        </View>
        <View style = {this.state.isHighContrast? [this.styles.enabled, {backgroundColor: this.state.highContrastColorValues.HighlightTextColor}] : this.styles.disabled}> 
          <Text>HighlightText High Contrast Color Hex Value: {this.state.highContrastColorValues.HighlightTextColor}</Text>
        </View>
        <View style = {this.state.isHighContrast? [this.styles.enabled, {backgroundColor: this.state.highContrastColorValues.HotlightColor}] : this.styles.disabled}> 
          <Text>Hotlight High Contrast Color Hex Value: {this.state.highContrastColorValues.HotlightColor}</Text>
        </View>
        <View style = {this.state.isHighContrast? [this.styles.enabled, {backgroundColor: this.state.highContrastColorValues.WindowColor}] : this.styles.disabled}> 
          <Text>Window High Contrast Color Hex Value: {this.state.highContrastColorValues.WindowColor}</Text>
        </View>
        <View style = {this.state.isHighContrast? [this.styles.enabled, {backgroundColor: this.state.highContrastColorValues.WindowTextColor}] : this.styles.disabled}> 
          <Text>WindowText High Contrast Color Hex Value: {this.state.highContrastColorValues.WindowTextColor}</Text>
        </View>
      </View>
    );
  }

  styles = StyleSheet.create ({
    enabled: {
      width: 250, 
      height: 50
    },
    disabled: {
      width: 250,
      height: 50,
      backgroundColor: '#808080'
    }
  });
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

class AccessibilityStateExamples extends React.Component {
  public state = {
    viewDisabled: false,
    itemsSelected: [false, false, false],
  }

  public render() {
    var selectableItems = [{}, {}, {}]
    return (
      <View>
        <Text>The following TouchableHighlight toggles accessibilityState.disabled for the View under it:</Text>
        <TouchableHighlight
          style={{width:100, height:50, backgroundColor:'blue'}}
          accessibilityRole="button"
          onPress={this.disablePress}
        >
          <Text>Toggle</Text>
        </TouchableHighlight>
        <View
          style={{backgroundColor: this.state.viewDisabled ? 'gray' : 'lightskyblue'}}
          accessibilityRole="text"
          accessibilityStates={this.state.viewDisabled ? ['disabled'] : []}>
          <Text>This View should be {this.state.viewDisabled ? "disabled" : "enabled"} according to UIA</Text>
        </View>
        <Text>The following list of TouchableHighlights toggles accessibilityState.selected when touched:</Text>
        <View accessibilityLabel="List of selectable items">
          <FlatList
            data={selectableItems}
            renderItem={(item) =>
              <TouchableHighlight
                style={{width:100, height:50, backgroundColor: this.state.itemsSelected[item.index] ? 'gray' : 'lightskyblue'}}
                accessibilityRole="button"
                accessibilityLabel={"Selectable item " + (item.index + 1)}
                accessibilityStates={this.state.itemsSelected[item.index] ? ['selected'] : []}
                onPress={() => this.selectPress(item.index)}
              >
                <Text>{this.state.itemsSelected[item.index] ? "Selected" : "Unselected"} </Text>
              </TouchableHighlight>
            }
            keyExtractor={(item, index) => index.toString()}
          />
        </View>
      </View>
    );
  }

  private disablePress = () => {
    this.setState({viewDisabled: !this.state.viewDisabled});
}

  private selectPress = (index: number) => {
    let tmp = this.state.itemsSelected;
    tmp[index] = !tmp[index];
    this.setState({itemsSelected: tmp});
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
  },
  {
    title: 'States',
    render: function(): JSX.Element {
      return <AccessibilityStateExamples />;
    },
  }
];