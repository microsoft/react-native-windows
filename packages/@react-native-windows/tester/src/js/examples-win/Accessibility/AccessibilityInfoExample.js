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
  TouchableWithoutFeedback,
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

function TouchableExample(props): React.Node {
  let myElement = React.createRef();

  return (
    <View>
      <TouchableWithoutFeedback
        onPress={() => {
          const reactTag = findNodeHandle(myElement.current);
          if (reactTag) {
            AccessibilityInfo.setAccessibilityFocus(reactTag);
          }
        }}
        accessibilityLabel="Test Setting Accessibility Focus"
        {...{
          focusable: true,
        }}>
        <View>
          <Text>Test setAccessibilityFocus</Text>
        </View>
      </TouchableWithoutFeedback>
      <TouchableWithoutFeedback
        ref={myElement}
        onPress={() => {
          //onPress, even if empty, is required for TouchableWithoutFocus to be focusable
        }}
        accessibilityLabel="focus received"
        {...{
          focusable: true,
        }}>
        <View>
          {/* When using Text, must wrap it within a View to receive keyboard focus*/}
          <Text>Testing touchablewithoutfeedback focusability</Text>
        </View>
      </TouchableWithoutFeedback>
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
  {
    title: 'setAccessibilityFocus on TouchableWithoutFeedback',
    render(): React.Element<typeof TouchableExample> {
      return <TouchableExample />;
    },
  },
];
