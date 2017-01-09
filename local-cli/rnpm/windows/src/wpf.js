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

module.exports = function windows(config, args, options) {
  const name = args[0] ? args[0] : Common.getReactNativeAppName();
  const ns = options.namespace ? options.namespace : name;
  const version = options.windowsVersion ? options.windowsVersion : Common.getReactNativeVersion();

  return Common.getInstallPackage(version)
    .then(rnwPackage => {
      console.log(`Installing ${rnwPackage}...`);
      // execSync(`npm install --save ${rnwPackage}`);
      console.log(chalk.green(`${rnwPackage} successfully installed.`));

      const generateWindows = require(Common.REACT_NATIVE_WPF_GENERATE_PATH());
      generateWindows(process.cwd(), name, ns);
    }).catch(error => console.error(chalk.red(error.message)));
}
