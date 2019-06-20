// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { Text, TouchableHighlight, View } from 'react-native';
import { Theming } from '../../src/index.uwp';

// 'global' var so that background changed on highContrast event can be easily 
// propogated to any control.
var currentBackgroundColor = ['blue', 'red'];
var isHighContrast = 'false';

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
  public render() {
    return (
      <View>
        <Text>The following has HighContrast Event awareness:</Text>
        <View
          style={{width:50, height:50, backgroundColor: currentBackgroundColor[0]}}
          accessibilityLabel="A blue box"
          accessibilityHint="A hint for the blue box.">
        <Text>{isHighContrast}</Text>
        </View>
        <Text>The following does not have HighContrast Event awareness:</Text>
        <View
          style={{ width: 50, height: 50, backgroundColor: currentBackgroundColor[1]}}
          accessible={true}
          accessibilityLabel="A hint for the red box."
        />
        onHighContrastChanged={Theming.addListener};
      </View>
    );
  }

  private onHighContrastChanged = () => {
      isHighContrast=Theming.getConstants();
      // Just changing both so that we can verify that control 1 
      // does change and control 2 does not.
      currentBackgroundColor=['white', 'black'];
  };
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
