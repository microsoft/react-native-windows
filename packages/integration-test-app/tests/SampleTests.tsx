/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import React from 'react';
import {assert} from 'chai';
import {Image, Platform} from 'react-native';
import {functionTest, componentTest} from './lib/TestDefinition';

/**
 * An example of a passing test
 */
functionTest('NativeModuleExample', () => {
  assert.equal(Platform.OS, 'windows', 'Platform.OS should be windows');
});

/**
 * An example of a failing test using ".skip" not to run
 */
functionTest.skip('FailingExample', () => {
  assert.isTrue(false, 'This test should fail');
});

/**
 * Tests can be written as React Components which call callbacks
 */
componentTest('ComponentExample', ({pass, fail}) => {
  return (
    <Image
      source={require('react-native-windows/IntegrationTests/blue_square.png')}
      onLoad={evt => {
        assert.ok(evt.nativeEvent.source);
        pass();
      }}
      onError={() => fail("Couldn't load image")}
    />
  );
});
