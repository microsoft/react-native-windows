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
import {StyleSheet, Text} from 'react-native';

const styles = StyleSheet.create({
  highlight: {
    fontWeight: '700',
  },
});

const ReloadInstructions = () => (
  <Text>
    Press <Text style={styles.highlight}>Ctrl+Shift+D</Text> and select{' '}
    <Text style={styles.highlight}>Reload javascript</Text> to reload your app's
    code.
  </Text>
);

export default ReloadInstructions;
