/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 */

import * as React from 'react'
import { useState } from 'react';
import { Text, View, StyleSheet, Button, findNodeHandle } from 'react-native';
import {
  MEASURE_IN_WINDOW_BUTTON,
  MEASURE_LAYOUT_BUTTON,
  DIRECT_MANIPULATION_RESULT,
} from './Consts';

const styles = StyleSheet.create({
  container: {
    padding: 20,
  },
  text: {
    textAlign: 'center',
    fontWeight: '700',
    height: 30,
  },
  childView: {
    width: 50,
    height: 50,
    backgroundColor: 'lightgreen',
    marginBottom: 20,
  },
});

const rootViewRef = React.createRef<View>();
const childViewRef = React.createRef<View>();

export function DirectManipulationTestPage() {
  const [resultTextState, setResultTextState] = useState('');

  const measureLayoutSucceeded = (
    x: number,
    y: number,
    width: number,
    height: number
  ) => {
    setResultTextState(
      `x=${x};y=${y};width=${Math.trunc(width)};height=${Math.trunc(height)}`
    );
  };

  const measureLayoutFailed = () => {
    setResultTextState('MeasureLayout failed');
  };

  return (
    <View ref={rootViewRef} style={styles.container}>
      <Text style={styles.text}>DirectManipulationResult:</Text>
      <Text testID={DIRECT_MANIPULATION_RESULT}>{resultTextState}</Text>
      <View style={styles.childView} ref={childViewRef} />
      <Button
        title="Call MeasureInWindow"
        onPress={() => {
          rootViewRef.current &&
            rootViewRef.current.measureInWindow((x, y, width, height) => {
              setResultTextState(
                `x=${x};y=${y};width=${width};height=${height}`
              );
            });
        }}
        testID={MEASURE_IN_WINDOW_BUTTON}
      />

      <Button
        title="Call MeasureLayout"
        onPress={() => {
          if (childViewRef.current) {
            const rootViewHandle = findNodeHandle(rootViewRef.current);
            if (rootViewHandle) {
              childViewRef.current.measureLayout(
                rootViewHandle,
                measureLayoutSucceeded,
                measureLayoutFailed
              );
            }
          }
        }}
        testID={MEASURE_LAYOUT_BUTTON}
      />
    </View>
  );
}

export const displayName = (_undefined?: string) => {};
export const title = '<LegacyDirectManipulationTest>';
export const description = 'Legacy e2e test for DirectManipulation';
export const examples = [
  {
    render: function(): JSX.Element {
      return <DirectManipulationTestPage />;
    },
  },
];