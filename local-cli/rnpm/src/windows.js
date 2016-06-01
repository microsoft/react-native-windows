/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

'use strict';

const childProcess = require('child_process');
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
}

const REACT_NATIVE_PACKAGE_JSON_PATH = function() {
  return path.resolve(
    process.cwd(),
    'node_modules',
    'react-native',
    'package.json'
  );
}

function getInstallPackage(version, safe) {
  var packageToInstall = 'react-native-windows';
  var validSemver = semver.valid(version);
  if (validSemver) {
    packageToInstall += '@' + validSemver;
  } else if (safe) {
    packageToInstall += '@' + version;
  } else if (version) {
    // for tar.gz or alternative paths
    packageToInstall = version;
  }
  return packageToInstall;
}

function getReactNativeVersion() {
  if (fs.existsSync(REACT_NATIVE_PACKAGE_JSON_PATH())) {
    var version = JSON.parse(fs.readFileSync(REACT_NATIVE_PACKAGE_JSON_PATH(), 'utf-8')).version;
    return semver.major(version) + '.' + semver.minor(version) + '.*';
  }
}

module.exports = function windows(config, args, options) {
  let name = args[0] ? args[0] : JSON.parse(fs.readFileSync('package.json', 'utf8')).name
  let ns = options['namespace'] ? options['namespace'] : name;
  
  let version = options['windowsVersion'] ? options['windowsVersion'] : null;
  let rnwPackage = version ? getInstallPackage(version) : getInstallPackage(getReactNativeVersion(), true);
  
  console.log('Installing ' + rnwPackage + '...');
  childProcess.execSync('npm install --save ' + rnwPackage);
  console.log(rnwPackage + ' successfully installed.');
  
  let generateWindows = require(REACT_NATIVE_WINDOWS_GENERATE_PATH());
  generateWindows(process.cwd(), name, ns);
}
