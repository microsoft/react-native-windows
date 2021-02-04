/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @format
 * @flow
 */

'use strict';

const React = require('react');
const ReactNative = require('react-native');

import {ColorGradientWin32} from '@office-iss/react-native-win32';

const {StyleSheet, Text, View} = ReactNative;

function VerticalToTransparentExample() {
  const color = {
    label: `ColorGradientWin32({
        gradientDirection: 'ToBottom',
        colorStops: [
          {color: 'green', offset: 0},
          {color: 'transparent', offset: 1},
        ],
      })`,
    color: ColorGradientWin32({
      gradientDirection: 'ToBottom',
      colorStops: [
        {color: 'green', offset: 0},
        {color: 'transparent', offset: 1},
      ],
    }),
  };

  return (
    <View style={styles.column}>
      <View style={styles.row}>
        <Text style={styles.labelCell}>{color.label}</Text>
        <View
          style={{
            ...styles.colorCell,
            backgroundColor: color.color,
          }}
        />
      </View>
    </View>
  );
}

function ThreeColorsHorizontalExample() {
  const color = {
    label: `ColorGradientWin32({
        gradientDirection: 'ToRight',
        colorStops: [
          {color: 'red', offset: 0},
          {color: 'blue', offset: 1},
        ],
      })`,
    color: ColorGradientWin32({
      gradientDirection: 'ToRight',
      colorStops: [
        {color: 'red', offset: 0},
        {color: 'blue', offset: 1},
      ],
    }),
  };

  return (
    <View style={styles.column}>
      <View style={styles.row}>
        <Text style={styles.labelCell}>{color.label}</Text>
        <View
          style={{
            ...styles.colorCell,
            backgroundColor: color.color,
          }}
        />
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  column: {flex: 1, flexDirection: 'column'},
  row: {flex: 0.75, flexDirection: 'row'},
  labelCell: {
    flex: 1,
    alignItems: 'stretch',
    color: 'black',
  },
  colorCell: {flex: 0.25, alignItems: 'stretch'},
});

exports.title = 'ColorGradientWin32';
exports.description = 'Examples using ColorGradientWin32.';
exports.examples = [
  {
    title: 'Vertical To Transparent',
    render(): React.Element<any> {
      return <VerticalToTransparentExample />;
    },
  },
  {
    title: 'Colors Horizontal',
    render(): React.Element<any> {
      return <ThreeColorsHorizontalExample />;
    },
  },
];
