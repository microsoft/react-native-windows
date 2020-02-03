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

module.exports = async function (config, args, options) {
  try {
    const name = args[0] ? args[0] : Common.getReactNativeAppName();
    const ns = options.namespace ? options.namespace : name;
    let version = options.windowsVersion ? options.windowsVersion : Common.getReactNativeVersion();

    let template = options.template;
    if (!template) {
      const validVersion = semver.valid(version);
      const validRange = semver.validRange(version);
      // If the RN version is >= 0.60 we know they have to use vnext
      if ((validVersion && semver.gte(validVersion, '0.60')) ||
          (!validVersion && validRange && semver.ltr('0.59.1000', validRange))) {
            template = 'vnext';
          }

      // If the RN version is >= 0.59 then we need to query which version the user wants
      if (!template && ((validVersion && semver.gte(validVersion, '0.59.0')) ||
          (!validVersion && validRange && semver.ltr('0.58.1000', validRange)))) {
            template = (await prompts({
              type: 'select',
              name: 'template',
              message: 'What version of react-native-windows would you like to install?',
              choices: [
                { value: 'vnext', title: ' [1mLatest[22m      - High performance react-native-windows built on a shared C++ core from facebook (supports C++ or C#).' },
                { value: 'legacy', title: ' [1mLegacy[22m      - Older react-native-windows implementation - (C# only, react-native <= 0.59 only)' },
              ],
            })).template;
      }
    }

    let rnwPackage = await Common.getInstallPackage(version, template, !!template);

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
  }
};
