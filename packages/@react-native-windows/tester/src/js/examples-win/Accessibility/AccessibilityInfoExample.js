/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

const React = require('react');
const {
  AccessibilityInfo,
  Button,
  Text,
  View,
  TextInput,
  findNodeHandle,
} = require('react-native');

function announceMessage() {
  AccessibilityInfo.announceForAccessibility(
    'Announce For Accessibility has been called. This is a notification event.',
  );
}

function focusTextInput(element) {
  if (element && element.current) {
    const reactTag = findNodeHandle(element.current);
    if (reactTag) {
      AccessibilityInfo.setAccessibilityFocus(reactTag);
    }
  }
}

function AccessibilityInfoExample(props): React.Node {
  const [isReduceMotionEnabled, setIsReduceMotionEnabled] = React.useState(
    'unknown',
  );
  const [isScreenReaderEnabled, setIsScreenReaderEnabled] = React.useState(
    'unknown',
  );
  let myElement = React.createRef();

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
      <Button
        onPress={announceMessage}
        title="Call AccessibilityInfo.announceForAccessibility"
        accessibilityLabel="Call Accessibility Info dot announce For Accessibility"
      />
      <Button
        onPress={() => focusTextInput(myElement)}
        title="Call AccessibilityInfo.setAccessibilityFocus"
        accessibilityLabel="Call Accessibility Info dot set Accessibility Focus"
      />
      <TextInput ref={myElement} />
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
