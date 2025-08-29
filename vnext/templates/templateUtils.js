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
const dependencyConfig = require('@react-native-windows/cli').dependencyConfig;

function getRnwInfo(config = {}, options = {}) {
  const projectRoot = config?.root ?? process.cwd();

  const rnwPath = path.dirname(
    require.resolve('react-native-windows', [projectRoot]),
  );

  const rnwVersion = require(path.join(rnwPath, 'package.json')).version;

  const devMode = existsSync(path.join(rnwPath, 'src-win'));

  const isCanary = rnwVersion.includes('canary');

  if (options?.logging) {
    console.log(
      `Found react-native-windows@${rnwVersion} at ${rnwPath}${
        devMode ? ' (devMode enabled)' : ''
      }...`,
    );
  }

  return {rnwPath, rnwVersion, devMode, isCanary};
}

function getWindowsProjectConfig(root) {
  if (!existsSync(root)) {
    return {};
  }

  const userConfigPath = path.join(root, 'react-native.config.js');

  const userConfig = existsSync(userConfigPath) ? require(userConfigPath) : {};

  return projectConfig(root, userConfig);
}

function getWindowsDependencyConfig(root) {
  if (!existsSync(root)) {
    return {};
  }

  const userConfigPath = path.join(root, 'react-native.config.js');

  const userConfig = existsSync(userConfigPath) ? require(userConfigPath) : {};

  return dependencyConfig(root, userConfig);
}

function pascalCase(str) {
  const camelCase = _.camelCase(str);
  return camelCase[0].toUpperCase() + camelCase.substr(1);
}

function generateDefaultModuleName(projectName) {
  // Convert project names like "react-native-webview" to "RNCWebviewModule"
  // or "my-awesome-library" to "MyAwesomeLibraryModule"
  
  // Remove common prefixes
  let cleanName = projectName;
  if (cleanName.startsWith('react-native-')) {
    cleanName = cleanName.substring('react-native-'.length);
    // For react-native- prefixed packages, use RNC prefix
    const pascalCleanName = pascalCase(cleanName);
    return `RNC${pascalCleanName}Module`;
  }
  
  // For other packages, just add Module suffix
  const pascalCleanName = pascalCase(cleanName);
  return `${pascalCleanName}Module`;
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
  const projectRoot = config?.root ?? process.cwd();

  if (options?.logging) {
    console.log(`Installing dependencies for ${projectRoot}...`);
  }
  const isYarn = existsSync(path.join(projectRoot, 'yarn.lock'));
  const cmd = isYarn ? 'yarn' : 'npm i';

  try {
    await exec(cmd, options?.logging ? {stdio: 'inherit'} : {});
  } catch (ex) {
    console.log(
      `Failed to install dependencies for ${projectRoot}. Please run '${cmd}' manually to update the dependencies.`,
    );
  }
}

async function updateProjectPackageJson(
  config = {},
  options = {},
  props = {},
  saveOptions = true,
  ensureWindowsInFiles = false,
) {
  const projectRoot = config?.root ?? process.cwd();
  const projectPackage =
    await pkgUtils.WritableNpmPackage.fromPath(projectRoot);

  if (!projectPackage) {
    throw new Error(
      `The directory '${projectRoot}' is not the root of an npm package`,
    );
  }

  if (saveOptions) {
    props['react-native-windows'] = {
      'init-windows': {
        name: options.name,
        namespace: options.namespace,
        template: options.template,
        moduleName: options.moduleName,
      },
    };
  }

  if (options?.logging) {
    console.log(`Modifying ${path.join(projectRoot, 'package.json')}...`);
  }

  // add "windows" to files if property exists
  if (ensureWindowsInFiles && projectPackage.json.files !== undefined) {
    props.files = projectPackage.json.files;
    if (!props.files.includes('windows')) {
      props.files.push('windows');
    }
  }

  await projectPackage.mergeProps(props);
}

module.exports = {
  getRnwInfo,
  getWindowsProjectConfig,
  getWindowsDependencyConfig,
  pascalCase,
  generateDefaultModuleName,
  replaceInFile,
  runNpmInstall,
  updateProjectPackageJson,
};
