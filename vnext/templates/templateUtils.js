/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 *
 * @ts check
 * @format
 */

const existsSync = require('fs').existsSync;
const fs = require('fs').promises;
const path = require('path');
const util = require('util');
const exec = util.promisify(require('child_process').exec);

const _ = require('lodash');

const pkgUtils = require('@react-native-windows/package-utils');

const projectConfig = require('@react-native-windows/cli').projectConfig;

function getRnwInfo(config = {}, options = {}) {
  const projectPath = config?.root ?? process.cwd();

  const rnwPath = path.dirname(
    require.resolve('react-native-windows', [projectPath]),
  );

  const rnwVersion = require(path.join(rnwPath, 'package.json')).version;

  const devMode = existsSync(path.join(rnwPath, 'src'));

  if (options?.logging) {
    console.log(
      `Found react-native-windows@${rnwVersion} at ${rnwPath}${
        devMode ? ' (devMode enabled)' : ''
      }...`,
    );
  }

  return {rnwPath, rnwVersion, devMode};
}

function getWindowsProjectConfig(root) {
  if (!existsSync(root)) {
    return {};
  }

  const userConfigPath = path.join(root, 'react-native.config.js');

  const userConfig = existsSync(userConfigPath) ? require(userConfigPath) : {};

  return projectConfig(root, userConfig);
}

function pascalCase(str) {
  const camelCase = _.camelCase(str);
  return camelCase[0].toUpperCase() + camelCase.substr(1);
}

async function replaceInFile(
  config = {},
  options = {},
  relativePath = '',
  searchValue = '',
  replaceValue = '',
) {
  const filePath = path.join(config?.root ?? process.cwd(), relativePath);
  if (existsSync(filePath)) {
    const contents = await fs.readFile(filePath, {encoding: 'utf-8'});
    const newContents = contents.replace(searchValue, replaceValue);
    if (contents !== newContents) {
      if (options?.logging) {
        console.log(`Modifying ${filePath}...`);
      }
      await fs.writeFile(filePath, newContents, {encoding: 'utf-8'});
    }
  }
}

async function runNpmInstall(config = {}, options = {}) {
  const projectPath = config?.root ?? process.cwd();

  if (options?.logging) {
    console.log(`Installing dependencies for ${projectPath}...`);
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
    console.log(`Modifying ${path.join(projectPath, 'package.json')}...`);
  }
  await projectPackage.mergeProps(props);
}

module.exports = {
  getRnwInfo,
  getWindowsProjectConfig,
  pascalCase,
  replaceInFile,
  runNpmInstall,
  updateProjectPackageJson,
};
