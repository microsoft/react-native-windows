/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {getNpmPackage} from '../PackageUtils';

test('getNpmPackage - Sanity Test', async () => {
  const npmPackage = await getNpmPackage();
  expect(npmPackage.name).toBe('react-native-platform-override');
});
