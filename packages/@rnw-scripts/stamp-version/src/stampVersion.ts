/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import {promises as fs} from 'fs';
import path from 'path';
import yargs from 'yargs';
import semver from 'semver';

import {findRepoPackage} from '@react-native-windows/package-utils';
import renderPropsFile from './renderPropsFile';

const {argv} = yargs
  .check(args => {
    if (args._.length === 1 && semver.valid(<string>args._[0])) {
      return true;
    } else {
      throw new Error('Usage: stamp-version <version>');
    }
  })
  .showHelpOnFail(false);

(async () => {
  const version = <string>argv._[0];
  await setPackageVersionProps(version);
})();

/**
 * Rewrites PackageVersion.g.props
 */
async function setPackageVersionProps(version: string) {
  const propsStr = await renderPropsFile(version);

  const rnwPackage = await findRepoPackage('react-native-windows');
  const propsPath = path.join(
    rnwPackage!.path,
    'PropertySheets/Generated/PackageVersion.g.props',
  );

  await fs.writeFile(propsPath, propsStr);
}
