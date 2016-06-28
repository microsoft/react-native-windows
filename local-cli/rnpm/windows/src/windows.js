/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

'use strict';

const chalk = require('chalk');
const execSync = require('child_process').execSync;
const fs = require('fs');
const path = require('path');
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

const REACT_NATIVE_PACKAGE_JSON_PATH = function() {
  return path.resolve(
    process.cwd(),
    'node_modules',
    'react-native',
    'package.json'
  );
};

function getInstallPackage(version) {
  let packageToInstall = 'react-native-windows';

  const validVersion = semver.valid(version);
  const validRange = semver.validRange(version);
  if ((validVersion && !semver.gtr(validVersion, '0.26.*')) ||
      (!validVersion && validRange && semver.gtr('0.27.0', validRange))) {
    console.error(
      'Please upgrade react-native to ^0.27 or specify a --windowsVersion that is >=0.27.0'
    );
    process.exit(1);
  }

  if (validVersion) {
    packageToInstall += '@' + validVersion;
  } else if (validRange) {
    packageToInstall += '@' + version;
  } else if (version) {
    // for tar.gz or alternative paths
    packageToInstall = version;
  }
  return packageToInstall;
}

function getReactNativeVersion() {
  if (fs.existsSync(REACT_NATIVE_PACKAGE_JSON_PATH())) {
    const version = JSON.parse(fs.readFileSync(REACT_NATIVE_PACKAGE_JSON_PATH(), 'utf-8')).version;
    return `${semver.major(version)}.${semver.minor(version)}.*`;
  }
}

module.exports = function windows(config, args, options) {
  const name = args[0] ? args[0] : JSON.parse(fs.readFileSync('package.json', 'utf8')).name;
  const ns = options.namespace ? options.namespace : name;
  const version = options.windowsVersion ? options.windowsVersion : getReactNativeVersion();

  const rnwPackage = getInstallPackage(version);
  console.log(chalk.green(`Installing ${rnwPackage}...`));
  execSync(`npm install --save ${rnwPackage}`);
  console.log(chalk.green(`${rnwPackage} successfully installed.`));

  const generateWindows = require(REACT_NATIVE_WINDOWS_GENERATE_PATH());
  generateWindows(process.cwd(), name, ns);
};
