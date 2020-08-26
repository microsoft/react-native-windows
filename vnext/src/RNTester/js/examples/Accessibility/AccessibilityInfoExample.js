/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

const React = require('react');
const {AccessibilityInfo, Button, Text, View} = require('react-native');

function announceSomething() {
  AccessibilityInfo.announceForAccessibility('Something!');
}

class AccessibilityInfoExample extends React.Component {
  state = {
    isReduceMotionEnabled: 'unknown',
    isScreenReaderEnabled: 'unknown',
  };

  componentDidMount() {
    AccessibilityInfo.isReduceMotionEnabled().done(isEnabled => {
      this.setState({isReduceMotionEnabled: isEnabled});
    });
    AccessibilityInfo.isScreenReaderEnabled().done(isEnabled => {
      this.setState({isScreenReaderEnabled: isEnabled});
    });
  }

  render() {
    return (
      <View>
        <Text>{`AccessibilityInfo.isReduceMotionEnabled: ${
          this.state.isReduceMotionEnabled
        }`}</Text>
        <Text>{`AccessibilityInfo.isScreenReaderEnabled: ${
          this.state.isScreenReaderEnabled
        }`}</Text>
        <Button onPress={announceSomething} title="Announce something" />
      </View>
    );
  }
}

exports.title = 'AccessibilityInfo';
exports.description = 'Examples of using AccessibilityInfo APIs.';
exports.examples = [
  {
    title: 'Basic AccessibilityInfo',
    render(): React.Element<typeof AccessibilityInfoExample> {
      return <AccessibilityInfoExample />;
    },
  },
];
