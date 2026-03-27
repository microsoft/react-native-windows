/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */

'use strict';

import React from 'react';
import { View, Text, Pressable } from 'react-native';

function HitTestWithOverflowVisibile() {
  const [bgColor, setBgColor] = React.useState('red');

  return (
    <View>
      <Text>
        Clicking the pressable should work even if it is outside the bounds
        of its parent.
      </Text>
      <View
        accessible={true}
        accessibilityValue={{ text: bgColor }}
        style={{ width: 150, height: 150 }}
        testID="visible-overflow-element">
        <View
          style={{
            width: 50,
            height: 50,
            backgroundColor: 'yellow',
            overflow: 'visible',
          }}>
          <Pressable
            style={{
              width: 100,
              height: 100,
              backgroundColor: bgColor,
            }}
            onPress={() => {
              setBgColor(bgColor === 'red' ? 'green' : 'red');
            }}>
            <Text>Press me</Text>
          </Pressable>
        </View>
      </View>
    </View>
  );
}

function HitTestWithOverflowHidden() {
  const [bgColor, setBgColor] = React.useState('red');
  return (
    <View>
      <Text>
        Clicking within the visible view will trigger the pressable.
        Clicking outside the bounds, where the pressable extends but is
        clipped by its parent overflow:hidden, should not trigger the
        pressable.
      </Text>
      <View
        accessible={true}
        accessibilityValue={{ text: bgColor }}
        style={{ width: 150, height: 150 }}
        testID="hidden-overflow-element">
        <View
          style={{
            width: 50,
            height: 50,
            backgroundColor: 'yellow',
            overflow: 'hidden',
          }}>
          <Pressable
            style={{
              width: 100,
              height: 100,
              backgroundColor: bgColor,
            }}
            onPress={() => {
              setBgColor(bgColor === 'red' ? 'green' : 'red');
            }}>
            <Text>Press me</Text>
          </Pressable>
        </View>
      </View>
    </View>
  );
}

exports.displayName = 'HitTestExample';
exports.title = 'Hit Testing';
exports.category = 'Basic';
exports.description = 'Test that overflow hidden affect hit testing';
exports.examples = [
  {
    title: 'overflow visible affects hit testing\n',
    render: function () {
      return <HitTestWithOverflowVisibile />;
    },
  },
  {
    title: 'overflow hidden affects hit testing\n',
    render: function () {
      return <HitTestWithOverflowHidden />;
    },
  },
];
