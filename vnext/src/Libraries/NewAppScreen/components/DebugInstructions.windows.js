/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 *
 * @flow strict-local
 * @format
 */

import React from 'react';
import type {Node} from 'react';
import {StyleSheet, Text} from 'react-native';

const styles = StyleSheet.create({
  highlight: {
    fontWeight: '700',
  },
});

const DebugInstructions = (): Node => (
  <Text>
    Press <Text style={styles.highlight}>Ctrl+Shift+D</Text> to open the React
    Native debug menu.
  </Text>
);

export default DebugInstructions;
