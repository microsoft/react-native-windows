/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */

import path from 'path';

test('Verify dependencies used for types is updated when cli versions in core update', async () => {
  const rnPath = path.dirname(require.resolve('react-native/package.json'));
  const rnCliPath = path.dirname(
    require.resolve('@react-native-community/cli', {paths: [rnPath]}),
  );
  const pkgJson = require(path.resolve(__dirname, '../../package.json'));

  const cliDrPkJson = require(require.resolve(
    '@react-native-community/cli-doctor/package.json',
    {paths: [rnCliPath]},
  ));
  expect(pkgJson.devDependencies['@react-native-community/cli-doctor']).toEqual(
    cliDrPkJson.version,
  );

  const cliTypesPkJson = require(require.resolve(
    '@react-native-community/cli-types/package.json',
    {paths: [rnCliPath]},
  ));
  expect(pkgJson.devDependencies['@react-native-community/cli-types']).toEqual(
    cliTypesPkJson.version,
  );
});
