/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @flow strict-local
 * @format
 */

'use strict';

import type {RNTesterModuleExample} from '../../types/RNTesterTypes';

const React = require('react');
const {StyleSheet, Text, View} = require('react-native');

function buttonLabel(button: number): string {
  switch (button) {
    case 0:
      return 'Left';
    case 1:
      return 'Middle';
    case 2:
      return 'Right';
    case 3:
      return 'X1';
    case 4:
      return 'X2';
    default:
      return `Unknown(${button})`;
  }
}

function PointerDownButtonExample(): React.Node {
  const [text, setText] = React.useState(
    'Click the box to test pointer events',
  );
  return (
    <View>
      <Text accessible={true} testID="pointer-button-state">
        {text}
      </Text>
      <View
        accessible={true}
        testID="pointer-button-target"
        style={styles.targetBox}
        onPointerDown={(e: any) => {
          const {button, buttons} = e.nativeEvent;
          setText(
            `PointerDown: ${buttonLabel(button)} (button=${button}, buttons=${buttons})`,
          );
        }}
      />
    </View>
  );
}

function PointerUpButtonExample(): React.Node {
  const [text, setText] = React.useState(
    'Click the box to test pointer up events',
  );
  return (
    <View>
      <Text accessible={true} testID="pointer-up-button-state">
        {text}
      </Text>
      <View
        accessible={true}
        testID="pointer-up-button-target"
        style={styles.targetBox}
        onPointerUp={(e: any) => {
          const {button, buttons} = e.nativeEvent;
          setText(
            `PointerUp: ${buttonLabel(button)} (button=${button}, buttons=${buttons})`,
          );
        }}
      />
    </View>
  );
}

exports.displayName = 'PointerButtonExample';
exports.framework = 'React';
exports.category = 'Basic';
exports.title = 'Pointer Button';
exports.documentationURL =
  'https://developer.mozilla.org/docs/Web/API/PointerEvent/button';
exports.description =
  'Tests that PointerEvent.button and PointerEvent.buttons are correctly populated.';

exports.examples = [
  {
    title: 'onPointerDown button property',
    description:
      'Click the box to verify the button property on onPointerDown events.',
    render: function (): React.Node {
      return <PointerDownButtonExample />;
    },
  },
  {
    title: 'onPointerUp button property',
    description:
      'Click the box to verify the button property on onPointerUp events.',
    render: function (): React.Node {
      return <PointerUpButtonExample />;
    },
  },
];

const styles = StyleSheet.create({
  targetBox: {
    backgroundColor: 'magenta',
    width: 200,
    height: 200,
    margin: 10,
    borderRadius: 10,
    justifyContent: 'center',
    alignItems: 'center',
  },
});
