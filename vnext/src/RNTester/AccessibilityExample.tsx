// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { Text, TouchableHighlight, View } from 'react-native';

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
    disabled: false,
  }

  public render() {
    return (
      <View>
        <Text>The following TouchableHighlight toggles accessibilityState.disabled for the View under it:</Text>
        <TouchableHighlight
          style={{width:50, height:50, backgroundColor:'blue'}}
          accessibilityRole="button"
          onPress={this.press}
        >
          <Text>Toggle</Text>
        </TouchableHighlight>
        <View
          style={{backgroundColor: this.state.disabled ? 'gray' : 'lightskyblue'}}
          accessibilityStates={this.state.disabled ? ['disabled'] : []}>
          <Text>This View should be {this.state.disabled ? "disabled" : "enabled"} according to UIA</Text>
        </View>
      </View>
    );
  }

  private press = () => {
    this.setState({disabled: !this.state.disabled});
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
    title: 'States',
    render: function(): JSX.Element {
      return <AccessibilityStateExamples />;
    },
  }
];
