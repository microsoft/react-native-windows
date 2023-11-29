/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

const existsSync = require('fs').existsSync;
const path = require('path');
const util = require('util');
const exec = util.promisify(require('child_process').exec);

const pkgUtils = require('@react-native-windows/package-utils');

async function runNpmInstall(config = {}, options = {}) {
  const projectPath = config?.root ?? process.cwd();

  if (options?.logging) {
    console.log('Installing dependencies...');
  }
  const isYarn = existsSync(path.join(projectPath, 'yarn.lock'));
  await exec(
    isYarn ? 'yarn' : 'npm i',
    options?.logging ? {stdio: 'inherit'} : {},
  );
}

async function updateProjectPackageJson(config = {}, options = {}, props = {}) {
  const projectPath = config?.root ?? process.cwd();
  const projectPackage = await pkgUtils.WritableNpmPackage.fromPath(
    projectPath,
  );

  if (!projectPackage) {
    throw new Error(
      `The directory '${projectPath}' is not the root of an npm package`,
    );
  }

  if (options?.logging) {
    console.log('Modifying project package.json...');
  }
  await projectPackage.mergeProps(props);
}

module.exports = {runNpmInstall, updateProjectPackageJson};
