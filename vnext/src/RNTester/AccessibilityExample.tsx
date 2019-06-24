// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

/* tslint:disable */

import React = require('react');
import { FlatList, Text, TouchableHighlight, View } from 'react-native';

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
        <Text accessibilityLiveRegion="polite" accessibilityLabel={"Pressed " + this.state.pressedCount + " times"}>Pressed {this.state.pressedCount} times</Text>
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
          style={{width:50, height:50, backgroundColor:'blue'}}
          accessibilityRole="button"
          onPress={this.disablePress}
        >
          <Text>Toggle</Text>
        </TouchableHighlight>
        <View
          style={{backgroundColor: this.state.viewDisabled ? 'gray' : 'lightskyblue'}}
          accessibilityStates={this.state.viewDisabled ? ['disabled'] : []}>
          <Text>This View should be {this.state.viewDisabled ? "disabled" : "enabled"} according to UIA</Text>
        </View>
        <Text>The following list of TouchableHighlights toggles accessibilityState.selected when touched:</Text>
        <View accessibilityLabel="List of selectable items" accessibilityRole={"header"}>
          <FlatList
            data={selectableItems}
            renderItem={(item) =>
              <TouchableHighlight
                style={{width:50, height:50, backgroundColor: this.state.itemsSelected[item.index] ? 'gray' : 'lightskyblue'}}
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
    title: 'States',
    render: function(): JSX.Element {
      return <AccessibilityStateExamples />;
    },
  }
];
