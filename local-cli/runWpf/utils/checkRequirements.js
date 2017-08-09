'use strict';

const execSync = require('child_process').execSync;
const path = require('path');
const shell = require('shelljs');
const Version = require('./version');

const REQUIRED_VERSIONS = {
  '10.0': {
    os: '6.3',
    msbuild: '14.0',
    visualstudio: '14.0',
    windowssdk: '8.1',
    phonesdk: '10.0'
  }
};

function shortenVersion (version) {
  return /^(\d+(?:\.\d+)?)/.exec(version.toString())[1];
}

function getMinimalRequiredVersionFor (requirement, windowsTargetVersion) {
  return Version.tryParse(REQUIRED_VERSIONS[windowsTargetVersion][requirement]);
}

function getInstalledWindowsSdks () {
  const installedSdks = [];

  const execString = 'reg query "HKLM\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows" /s /v InstallationFolder /reg:32';
  let output;
  try {
    output = execSync(execString).toString();
  } catch (e) {
    return installedSdks;
  }

  const re = /\\Microsoft SDKs\\Windows\\v(\d+\.\d+)\s*InstallationFolder\s+REG_SZ\s+(.*)/gim;
  let match;
  while ((match = re.exec(output))) {
    const sdkPath = match[2];
    if (shell.test('-e', path.join(sdkPath, 'SDKManifest.xml'))) {
      installedSdks.push(Version.tryParse(match[1]));
    }
  }

  return installedSdks;
}

function checkWinSdk (windowsTargetVersion) {
  const installedSdks = getInstalledWindowsSdks();
  const requiredVersion = getMinimalRequiredVersionFor('windowssdk', windowsTargetVersion);
  const hasSdkInstalled = installedSdks.some(installedSdk => installedSdk.eq(requiredVersion));

  if (hasSdkInstalled) { return shortenVersion(requiredVersion); }

  if (!hasSdkInstalled) {
    const shortenedVersion = shortenVersion(requiredVersion);
    throw new Error(
      `Windows SDK not found. Ensure that you have installed Windows ${shortenedVersion} SDK along with Visual Studio or install Windows ${shortenedVersion} SDK separately from https://dev.windows.com/en-us/downloads`
    );
  }
}

module.exports.isWinSdkPresent = function isWinSdkPresent(target) {
  return checkWinSdk(target);
};
