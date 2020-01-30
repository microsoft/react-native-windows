/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
'use strict';

const Common = require('./common');
const chalk = require('chalk');
const execSync = require('child_process').execSync;
const path = require('path');
const prompt = require('@react-native-community/cli/build/tools/generator/promptSync').default();

const REACT_NATIVE_WINDOWS_GENERATE_PATH = function() {
  return path.resolve(
    process.cwd(),
    'node_modules',
    'react-native-windows',
    'local-cli',
    'generate-windows.js'
  );
};

module.exports = function (config, args, options) {
  const name = args[0] ? args[0] : Common.getReactNativeAppName();
  const ns = options.namespace ? options.namespace : name;
  const version = options.windowsVersion ? options.windowsVersion : Common.getReactNativeVersion();

  let template = options.template;
  if (!template) {
    console.log("What version of react-native-windows would you like to install?  Choose one of:  legacy, latest [default]:");
    template = prompt();
    if (template === '') {
      template = 'vnext'
    }
  }

  return Common.getInstallPackage(version, template, true)
    .then(rnwPackage => {
      console.log(`Installing ${rnwPackage}...`);
      const pkgmgr = Common.isGlobalCliUsingYarn(process.cwd()) ? 'yarn add' : 'npm install --save';

      const execOptions = options.verbose ? { stdio: 'inherit' } : {};
      execSync(`${pkgmgr} ${rnwPackage}`, execOptions);
      console.log(chalk.green(`${rnwPackage} successfully installed.`));

      const generateWindows = require(REACT_NATIVE_WINDOWS_GENERATE_PATH());
      generateWindows(process.cwd(), name, ns, options);
    }).catch(error => console.error(chalk.red(error.message)));
};
