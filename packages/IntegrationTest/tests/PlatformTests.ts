/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {assert} from 'chai';
import {Platform} from 'react-native';
import {functionTest} from './lib/TestDefinition';

functionTest('Platform.OS', () => {
  assert.equal(Platform.OS, 'windows', 'Platform.OS should be windows');
});

functionTest('Platform.Version', () => {
  assert.isNumber(Platform.Version, 'Platform.Version should be a number');
  assert.isAtLeast(
    Platform.Version as number,
    1,
    'Universal API contract should be at least 1',
  );
});

functionTest('PlatformConstants.reactNativeVersion', () => {
  // @ts-ignore constants not exposed in public typings
  const constants = Platform.constants;

  assert.isObject(
    constants.reactNativeVersion,
    'Platform.constants.reactNativeVersion should be an object',
  );
  assert.isNumber(
    constants.reactNativeVersion.major,
    'reactNativeVersion.major should be a number',
  );
  assert.isNumber(
    constants.reactNativeVersion.minor,
    'reactNativeVersion.minor should be a number',
  );
  assert.isNumber(
    constants.reactNativeVersion.patch,
    'reactNativeVersion.patch should be a number',
  );
});
