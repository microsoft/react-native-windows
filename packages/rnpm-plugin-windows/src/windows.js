/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
'use strict';

const Common = require('./common');
const chalk = require('chalk');
const execSync = require('child_process').execSync;
const path = require('path');
const prompts = require('prompts');
const semver = require('semver');

const REACT_NATIVE_WINDOWS_GENERATE_PATH = function() {
  return path.resolve(
    process.cwd(),
    'node_modules',
    'react-native-windows',
    'local-cli',
    'generate-windows.js'
  );
};

async function getDefaultVersionTag(version) {
  const validVersion = semver.valid(version);
  const validRange = semver.validRange(version);
  if (!validVersion && !validRange) {
    console.error(chalk.red(`'${version}' is not a valid version`));
    process.exit(1);
  }

  // 0.57 and below had stable untagged releases
  if ((validVersion && semver.lt(validVersion, '0.58.0'))
    || (validRange && semver.gtr('0.58.0', validRange))) {
    return null;
  }

  // 0.58 went to RC (See #2559)
  if ((validVersion && semver.lt(validVersion, '0.59.0'))
    || (validRange && semver.gtr('0.59.0', validRange))) {
    return 'rc';
  }

  // 0.59 tags releases as "legacy" or "vnext"
  if ((validVersion && semver.lt(validVersion, '0.60.0'))
    || (validRange && semver.gtr('0.60.0', validRange))) {
    return  (await prompts({
      type: 'select',
      name: 'template',
      message: 'What version of react-native-windows would you like to install?',
      choices: [
        { value: 'vnext', title: ' [1mLatest[22m      - High performance react-native-windows built on a shared C++ core from facebook (supports C++ or C#).' },
        { value: 'legacy', title: ' [1mLegacy[22m      - Older react-native-windows implementation - (C# only, react-native <= 0.59 only)' },
      ],
    })).template;
  }

  // 0.60 releases all use the vnext tag
  if ((validVersion && semver.lt(validVersion, '0.61.0'))
    || (validRange && semver.gtr('0.61.0', validRange))) {
    return 'vnext';
  }

  // 0.61 and after don't tag stable releases
  return null;
}

module.exports = async function (config, args, options) {
  try {
    const name = args[0] || Common.getReactNativeAppName();
    const ns = options.namespace || name;
    let version = options.windowsVersion || Common.getReactNativeVersion();
    const versionTag = options.template || await getDefaultVersionTag(version);

    let rnwPackage = await Common.getInstallPackage(version, versionTag);

    console.log(`Installing ${rnwPackage}...`);
    const pkgmgr = Common.isGlobalCliUsingYarn(process.cwd()) ? 'yarn add' : 'npm install --save';

    const execOptions = options.verbose ? { stdio: 'inherit' } : {};
    execSync(`${pkgmgr} ${rnwPackage}`, execOptions);
    console.log(chalk.green(`${rnwPackage} successfully installed.`));

    const generateWindows = require(REACT_NATIVE_WINDOWS_GENERATE_PATH());
    generateWindows(process.cwd(), name, ns, options);
  } catch (error) {
    console.error(chalk.red(error.message));
    console.error(error);
    process.exit(1);
  }
};
