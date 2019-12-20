/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import React = require('react');
import {StyleSheet, Button, Text, View} from 'react-native';

const styles = StyleSheet.create({
  withBorder: {
    borderRadius: 5,
  },
  withoutBorder: {
    direction: 'rtl',
  },
  zIndex: {
    justifyContent: 'space-around',
    width: 100,
    height: 50,
    marginTop: -10,
  },
  blankStyle: {},
  border: {
    borderRadius: 5,
  },
});

class FlowDirectionChange extends React.Component<{}, any> {
  state = {
    useBorder: false,
  };

  render() {
    const currentStyle = this.state.useBorder
      ? styles.withBorder
      : styles.withoutBorder;
    return (
      <View style={{flex: 1}}>
        <View style={[styles.withoutBorder, currentStyle]}>
          <Text style={{fontSize: 11}}>
            This text should remain on the right after changing the radius
          </Text>
        </View>
        <Button onPress={this.handleClick} title="Change Border Radius" />
      </View>
    );
  }

  handleClick = () => {
    this.setState({useBorder: !this.state.useBorder});
  };
}

class ZIndexChange extends React.Component<{}, any> {
  state = {
    cornerRadius: false,
    flipped: false,
  };

  render() {
    const hasRadius = this.state.cornerRadius
      ? styles.border
      : styles.blankStyle;
    const indices = this.state.flipped ? [-1, 0, 1, 2] : [2, 1, 0, -1];
    return (
      <View>
        <View>
          <Text style={{paddingBottom: 10}}>
            The zIndex should remain the same after changing the border radius
          </Text>
          <View
            testID="automationID"
            style={[
              styles.zIndex,
              {
                marginTop: 0,
                backgroundColor: '#E57373',
                zIndex: indices[0],
              },
              hasRadius,
            ]}>
            <Text>ZIndex {indices[0]}</Text>
          </View>
          <View
            style={[
              styles.zIndex,
              {
                marginLeft: 50,
                backgroundColor: '#FFF176',
                zIndex: indices[1],
              },
              hasRadius,
            ]}>
            <Text>ZIndex {indices[1]}</Text>
          </View>
          <View
            style={[
              styles.zIndex,
              {
                marginLeft: 100,
                backgroundColor: '#81C784',
                zIndex: indices[2],
              },
              hasRadius,
            ]}>
            <Text>ZIndex {indices[2]}</Text>
          </View>
          <View
            style={[
              styles.zIndex,
              {
                marginLeft: 150,
                backgroundColor: '#64B5F6',
                zIndex: indices[3],
              },
              hasRadius,
            ]}>
            <Text>ZIndex {indices[3]}</Text>
          </View>
        </View>
        <Button onPress={this.handleClick} title="Change Border Radius" />
        <Button onPress={this.handlePress} title="Change Sorting Order" />
      </View>
    );
  }

  handleClick = () => {
    this.setState({cornerRadius: !this.state.cornerRadius});
  };

  handlePress = () => {
    this.setState({flipped: !this.state.flipped});
  };
}

export const displayName = (_undefined?: string) => {};
export const title = '<TransferProperties>';
export const description =
  'Some tests that change the backing XAML element ' +
  'to see if transfer properties is working.';
export const examples = [
  {
    title: 'Flow Direction Change',
    render: function(): JSX.Element {
      return <FlowDirectionChange />;
    },
  },
  {
    title: 'zIndex Change',
    render: function(): JSX.Element {
      return <ZIndexChange />;
    },
  },
];
