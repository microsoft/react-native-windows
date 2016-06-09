'use strict';

var child_process = require('child_process');
var path = require('path');
var shell = require('shelljs');
var Version = require('./version');

var REQUIRED_VERSIONS = {
  '10.0': {
    os: '6.3',
    msbuild: '14.0',
    visualstudio: '14.0',
    windowssdk: '10.0',
    phonesdk: '10.0'
  }
}

function getMinimalRequiredVersionFor (requirement, windowsTargetVersion) {
  return Version.tryParse(REQUIRED_VERSIONS[windowsTargetVersion][requirement]);
}

function getInstalledWindowsSdks () {
  var installedSdks = [];

  var execString = 'reg query HKLM\\SOFTWARE\\Microsoft\\Microsoft SDKs\\Windows /s /s InstallationFolder /reg:32';
  var output;
  try {
    output = child_process.execSync(execString).toString();
  } catch (e) {
    return installedSdks;
  }

  var re = /\\Microsoft SDKs\\Windows\\v(\d+\.\d+)\s*InstallationFolder\s+REG_SZ\s+(.*)/gim;
  var match;
  while ((match = re.exec(output))) {
    var sdkPath = match[2];
    if (shell.test('-e', path.join(sdkPath, 'SDKManifest.xml'))) {
      installedSdks.push(Version.tryParse(match[1]));
    }
  }

  return installedSdks;
}

