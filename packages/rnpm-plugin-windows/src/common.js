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

function isTagMatch(packageVersion, requestTag) {
  const prerelease = semver.prerelease(packageVersion);
  if (prerelease === null && !requestTag) {
    return true;
  } else {
    return prerelease && prerelease[0] === requestTag;
  }
}

function isVersionMatch(packageVersion, requestVersion, requestTag) {
  if (semver.parse(packageVersion) === null) {
    return false;
  }

  const { major, minor } = semver.parse(packageVersion);
  const minVersion = semver.minVersion(requestVersion);
  return major === minVersion.major &&
    minor === minVersion.minor &&
    isTagMatch(packageVersion, requestTag);
}

function getLatestMatchingVersion(versionRange, tag) {
  return new Promise((resolve, reject) => {
    // Ignore the version range of React Native if asking for master, since our
    // RNW version may not align.
    if (tag === 'master') {
      npm.packages.release('react-native-windows', 'master', (err, rel) => {
        if (err) {
          reject(err);
        } else {
          resolve(rel[0].version);
        }
      });
    } else {
      npm.packages.releases('react-native-windows', (err, rels) => {
        if (err) {
          reject(err);
        } else {
          const matchingVersions = Object.keys(rels)
            .filter(v => isVersionMatch(v, versionRange, tag))
            .sort(semver.rcompare);

          if (matchingVersions.length > 0) {
            resolve(matchingVersions[0]);
          } else {
            reject();
          }
        }
      });
    }
  });
}

async function getMatchingVersion(versionRange, tag) {
  const versionStr = tag === 'master' ? 'master' : versionRange;
  console.log(`Checking for react-native-windows version matching ${versionStr}...`);

  try {
    return await getLatestMatchingVersion(versionRange, tag);
  } catch (ex) {
    const latestVersion = await getLatestVersion();
    throw new Error(`Could not find react-native-windows@${versionStr}. ` +
      `Latest version of react-native-windows is ${latestVersion}, try switching to ` +
      `react-native@${semver.major(latestVersion)}.${semver.minor(latestVersion)}.*.`);
  }
}

const isSupportedVersion = function(validVersion, validRange) {
  // Allow 0.0.0-x master builds
  if (validVersion) {
    return semver.lt(validVersion, '0.0.0') || semver.gte(validVersion, '0.27.0');
  } else if (validRange) {
    return semver.gtr('0.0.0', validRange) || semver.ltr('0.27.0', validRange);
  } else {
    return false;
  }
};

const getInstallPackage = function (version, tag) {
  const packageToInstall = 'react-native-windows';
  const validVersion = semver.valid(version);
  const validRange = semver.validRange(version);

  if (!isSupportedVersion(validVersion, validRange)) {
    console.error(
      'Please upgrade react-native to ^0.27 or specify a --windowsVersion that is >=0.27.0'
    );
    process.exit(1);
  }

  if (validVersion) {
    return Promise.resolve(`${packageToInstall}@${version}`);
  } else {
    return getMatchingVersion(version, tag)
      .then(resultVersion => `${packageToInstall}@${resultVersion}`);
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
  let name = JSON.parse(fs.readFileSync('package.json', 'utf8')).name;
  if (!name) {
    if (fs.existsSync('app.json')) {
      console.log('Reading application name from app.json...');
      name = JSON.parse(fs.readFileSync('app.json', 'utf8')).name;
    }
  }
  if (!name) {
    console.error('Please specify name in package.json or app.json');
  }
  return name;
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

module.exports = {
  getInstallPackage,
  getReactNativeVersion,
  getReactNativeAppName,
  isGlobalCliUsingYarn,
};
