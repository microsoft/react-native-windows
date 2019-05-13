/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
'use strict';

const fs = require('fs');
const path = require('path');
const semver = require('semver');
const Registry = require('npm-registry');
const child_process = require('child_process');
const validUrl = require('valid-url');
const request = require('request');
const fsextra = require('fs-extra');
var extract = require('extract-zip')

let npmConfReg = child_process.execSync('npm config get registry').toString().trim();
let NPM_REGISTRY_URL = validUrl.is_uri(npmConfReg) ? npmConfReg : 'http://registry.npmjs.org';

const REACT_NATIVE_PACKAGE_JSON_PATH = function() {
  return path.resolve(
    process.cwd(),
    'node_modules',
    'react-native',
    'package.json'
  );
};

const npm = new Registry({registry: NPM_REGISTRY_URL});

function getLatestVersion() {
  return new Promise(function (resolve, reject) {
    npm.packages.release('react-native-windows', 'latest', (err, releases) => {
      if (err) {
        reject(err);
      } else if (!releases || releases.length === 0) {
        reject(new Error('Could not find react-native-windows@latest.'));
      } else {
        resolve(releases[0].version);
      }
    });
  });
}

function getMatchingVersion(version) {
  console.log(`Checking for react-native-windows version matching ${version}...`);
  return new Promise(function (resolve, reject) {
    npm.packages.range('react-native-windows', version, (err, release) => {
      if (err || !release) {
        return getLatestVersion()
          .then(latestVersion => {
            reject(new Error(`Could not find react-native-windows@${version}. ` +
              `Latest version of react-native-windows is ${latestVersion}, try switching to ` +
              `react-native@${semver.major(latestVersion)}.${semver.minor(latestVersion)}.*.`));
            }).catch(error => reject(new Error(`Could not find react-native-windows@${version}.`)));
      } else {
        resolve(release.version);
      }
    });
  });
}

const getInstallPackage = function (version) {
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

  if (validVersion || validRange) {
    return getMatchingVersion(version)
      .then(resultVersion => packageToInstall + '@' + resultVersion);
  } else {
    return Promise.resolve(version);
  }
};

const getReactNativeVersion = function () {
  console.log('Reading react-native version from node_modules...');
  if (fs.existsSync(REACT_NATIVE_PACKAGE_JSON_PATH())) {
    const version = JSON.parse(fs.readFileSync(REACT_NATIVE_PACKAGE_JSON_PATH(), 'utf-8')).version;
    return `${semver.major(version)}.${semver.minor(version)}.*`;
  }
};

const getReactNativeAppName = function () {
  console.log('Reading application name from package.json...');
  return JSON.parse(fs.readFileSync('package.json', 'utf8')).name;
};

/**
 * Check that 'react-native init' itself used yarn to install React Native.
 * When using an old global react-native-cli@1.0.0 (or older), we don't want
 * to install React Native with npm, and React + Jest with yarn.
 * Let's be safe and not mix yarn and npm in a single project.
 * @param projectDir e.g. /Users/martin/AwesomeApp
 */
const isGlobalCliUsingYarn = function(projectDir) {
  return fs.existsSync(path.join(projectDir, 'yarn.lock'));
};

const downloadFile = function (filename, url) {
  return fsextra.ensureFile(filename).then(function () {
    return new Promise(function (resolve, reject) {
      request.get(url).pipe(fs.createWriteStream(filename)).on('close', resolve).on('error', function (err) {
        reject(err);
      });
    });
  });
};

const unzipFile = function (file, dir) {
  return new Promise(function (resolve, reject) {
    extract(file, { dir }, function (err) {
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
}

module.exports = {
  getInstallPackage,
  getReactNativeVersion,
  getReactNativeAppName,
  isGlobalCliUsingYarn,
  downloadFile,
  unzipFile
};
