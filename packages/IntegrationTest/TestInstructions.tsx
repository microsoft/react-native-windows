/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as React from 'react';
import {PlatformColor, StyleSheet, Text, View} from 'react-native';

const TestInstructions: React.FC = () => {
  return (
    <View style={styles.container}>
      <Text style={styles.mainMessage}>
        <Text>Run </Text>
        <Text style={styles.monospace}>yarn integration-test</Text>
        <Text> to get started</Text>
      </Text>
      <Text style={styles.subHeading}>
        See the README in the root of this package for details
      </Text>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
  mainMessage: {
    fontSize: 32,
    fontWeight: 'bold',
  },
  monospace: {
    fontFamily: 'consolas',
    fontWeight: 'bold',
    color: PlatformColor('SystemAccentColorLight2'),
  },
  subHeading: {
    paddingTop: 8,
    fontSize: 18,
  },
});

export default TestInstructions;
