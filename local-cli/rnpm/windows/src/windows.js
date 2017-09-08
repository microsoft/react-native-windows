/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

'use strict';

const Common = require('./common');
const chalk = require('chalk');
const execSync = require('child_process').execSync;
const path = require('path');

const REACT_NATIVE_WINDOWS_GENERATE_PATH = function() {
  return path.resolve(
    process.cwd(),
    'node_modules',
    'react-native-windows',
    'local-cli',
    'generate-windows.js'
  );
};

module.exports = function windows(config, args, options) {
  const name = args[0] ? args[0] : Common.getReactNativeAppName();
  const ns = options.namespace ? options.namespace : name;
  const version = options.windowsVersion ? options.windowsVersion : Common.getReactNativeVersion();

  return Common.getInstallPackage(version)
    .then(rnwPackage => {
      console.log(`Installing ${rnwPackage}...`);
      const pkgmgr = Common.isGlobalCliUsingYarn(process.cwd()) ? 'yarn add' : 'npm install --save';

      const execOptions = options.verbose ? { stdio: 'inherit' } : {};
      execSync(`${pkgmgr} ${rnwPackage}`, execOptions);
      console.log(chalk.green(`${rnwPackage} successfully installed.`));

      const generateWindows = require(REACT_NATIVE_WINDOWS_GENERATE_PATH());
      generateWindows(process.cwd(), name, ns);
    }).catch(error => console.error(chalk.red(error.message)));
};
