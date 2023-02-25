/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @format
 */

import fs from '@react-native-windows/fs';
import path from 'path';
import yargs from 'yargs';
import semver from 'semver';

import {findRepoPackage} from '@react-native-windows/package-utils';
import renderPropsFile from './renderPropsFile';

const {argv} = yargs
  .check(args => {
    if (args._.length === 2 && semver.valid(<string>args._[0])) {
      return true;
    } else {
      throw new Error('Usage: stamp-version <version> <commitid>');
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
  const commitId =
    process.env.BUILD_SOURCEVERSION ?? // Azure DevOps Pipelines
    process.env.GITHUB_SHA ?? // GitHub Actions
    'developer-build';
  const propsStr = await renderPropsFile(version, commitId);

  const rnwPackage = await findRepoPackage('react-native-windows');
  const propsPath = path.join(
    rnwPackage!.path,
    'PropertySheets/Generated/PackageVersion.g.props',
  );

  await fs.writeFile(propsPath, propsStr);
}
