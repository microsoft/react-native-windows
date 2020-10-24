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

function AccessibilityInfoExample(props): React.Node {
  const [isReduceMotionEnabled, setIsReduceMotionEnabled] = React.useState(
    'unknown',
  );
  const [isScreenReaderEnabled, setIsScreenReaderEnabled] = React.useState(
    'unknown',
  );

  React.useEffect(() => {
    AccessibilityInfo.isReduceMotionEnabled().done(isEnabled => {
      setIsReduceMotionEnabled(isEnabled);
    });
    AccessibilityInfo.isScreenReaderEnabled().done(isEnabled => {
      setIsScreenReaderEnabled(isEnabled);
    });
  }, [setIsReduceMotionEnabled, setIsScreenReaderEnabled]);

  return (
    <View>
      <Text>{`AccessibilityInfo.isReduceMotionEnabled: ${isReduceMotionEnabled}`}</Text>
      <Text>{`AccessibilityInfo.isScreenReaderEnabled: ${isScreenReaderEnabled}`}</Text>
      <Button onPress={announceSomething} title="Announce something" />
    </View>
  );
}

exports.title = 'AccessibilityInfo';
exports.category = 'Basic';
exports.description = 'Examples of using AccessibilityInfo APIs.';
exports.examples = [
  {
    title: 'Basic AccessibilityInfo',
    render(): React.Element<typeof AccessibilityInfoExample> {
      return <AccessibilityInfoExample />;
    },
  },
];
