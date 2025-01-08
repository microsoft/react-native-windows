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

  // Check for size limits. A valid package name has 100 characters or less.
  expect(
    nameUtils.isValidTelemetryPackageName(
      'react_native_react_native_react_native_react_native_react_native_react_native_react_native_react_nat',
    ),
  ).toBe(true);
  expect(
    nameUtils.isValidTelemetryPackageName(
      'react_native_react_native_react_native_react_native_react_native_react_native_react_native_react_nati',
    ),
  ).toBe(false);
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

  expect(
    nameUtils.cleanTelemetryPackageName(
      'react_native_react_native_react_native_react_native_react_native_react_native_react_native_react_nat',
    ),
  ).toBe(
    'react_native_react_native_react_native_react_native_react_native_react_native_react_native_react_nat',
  );

  // Truncate a package name with 101 characters, to the first 100.
  expect(
    nameUtils.cleanTelemetryPackageName(
      'react_native_react_native_react_native_react_native_react_native_react_native_react_native_react_nati',
    ),
  ).toBe(
    'react_native_react_native_react_native_react_native_react_native_react_native_react_native_react_nat',
  );
});
