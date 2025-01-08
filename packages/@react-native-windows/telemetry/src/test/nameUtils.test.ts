/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import * as nameUtils from '../utils/nameUtils';

test('Verify telemetry package name is valid', () => {
  expect(nameUtils.isValidTelemetryPackageName('package')).toBe(true);
  expect(nameUtils.isValidTelemetryPackageName('@react')).toBe(false);
  expect(nameUtils.isValidTelemetryPackageName('react-native')).toBe(false);
  expect(nameUtils.isValidTelemetryPackageName('react_native')).toBe(true);
  expect(nameUtils.isValidTelemetryPackageName('react_native/cli')).toBe(false);
});

test('Verify telemetry package name cleaning', () => {
  expect(nameUtils.cleanTelemetryPackageName('package')).toBe('package');
  expect(nameUtils.cleanTelemetryPackageName('@react')).toBe('_react');
  expect(nameUtils.cleanTelemetryPackageName('react-native')).toBe(
    'react_native',
  );
  expect(nameUtils.cleanTelemetryPackageName('react_native')).toBe(
    'react_native',
  );
  expect(nameUtils.cleanTelemetryPackageName('react_native/cli')).toBe(
    'react_native_cli',
  );
  expect(nameUtils.cleanTelemetryPackageName('@react-native-windows/cli')).toBe(
    '_react_native_windows_cli',
  );
});
