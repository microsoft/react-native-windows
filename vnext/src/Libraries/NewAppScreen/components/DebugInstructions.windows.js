/**
 * @flow
 * @format
 */

import React from 'react';
import {StyleSheet, Text} from 'react-native';

const styles = StyleSheet.create({
  highlight: {
    fontWeight: '700',
  },
});

const DebugInstructions = () => (
  <Text>
    Press <Text style={styles.highlight}>Ctrl+Shift+D</Text> to open the React
    Native debug menu.
  </Text>
);

export default DebugInstructions;
