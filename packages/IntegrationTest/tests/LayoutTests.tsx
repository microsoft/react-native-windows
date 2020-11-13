/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as React from 'react';
import {useEffect, useRef} from 'react';
import {assert} from 'chai';
import {NativeMethods, StyleSheet, View} from 'react-native';
import {componentTest} from './lib/TestDefinition';

function onNativeRender(callback: () => void) {
  // We need to wait until native has rendered a frame before measuring will
  // return non-zero results. Use RAF to schedule work on the next render, to
  // then shceduled work on the render after (at which point we should be all
  // good).
  requestAnimationFrame(() => {
    requestAnimationFrame(() => {
      callback();
    });
  });
}

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
    margin: 10,
  },
  scaledView: {
    transform: [{scale: 0.5}],
  },
});

componentTest('VerifyMeasure', ({pass}) => {
  const ref = useRef<React.Component<any> & Readonly<NativeMethods>>(null);
  useEffect(() => {
    onNativeRender(() => {
      ref.current!.measure((_x, _y, width, height) => {
        assert.equal(width, 50);
        assert.equal(height, 50);
        pass();
      });
    });
  });

  return <View style={styles.childView} ref={ref} />;
});

componentTest('VerifyMeasureInWindow', ({pass}) => {
  const ref = useRef<React.Component<any> & Readonly<NativeMethods>>(null);
  useEffect(() => {
    onNativeRender(() => {
      ref.current!.measureInWindow((_x, _y, width, height) => {
        assert.equal(width, 50);
        assert.equal(height, 50);
        pass();
      });
    });
  });

  return (
    <View style={styles.container}>
      <View style={styles.childView} ref={ref} />
    </View>
  );
});

componentTest('VerifyMeasureOnScaledComponent', ({pass}) => {
  const ref = useRef<React.Component<any> & Readonly<NativeMethods>>(null);
  useEffect(() => {
    onNativeRender(() => {
      // https://github.com/microsoft/react-native-windows/issues/6530: Currently RNW does not scale the result in measure... which matches react-native-web, but not android/iOS
      ref.current!.measure((_x, _y, width, height) => {
        assert.equal(
          width,
          50,
          'Components with 50% scale should scale width on measure call, so this value should be 25 but currently RNW does not scale the value - See Issue 6530',
        );
        assert.equal(
          height,
          50,
          'Components with 50% scale should scale height on measure call, so this value should be 25 but currently RNW does not scale the value - See Issue 6530',
        );
        pass();
      });
    });
  });

  return <View style={[styles.childView, styles.scaledView]} ref={ref} />;
});
