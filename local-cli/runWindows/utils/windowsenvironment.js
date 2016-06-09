'use strict';

var child_process = require('child_process');
var path = require('path');
var shell = require('shelljs');
var Version = require('./version');

function getMinimalRequiredVersionFor (requirement, windowsTargetVersion) {

    if (windowsTargetVersion === '8' || windowsTargetVersion === '8.0') {
        throw new CordovaError('windows8 platform is deprecated. To use windows-target-version=8.0 you may downgrade to cordova-windows@4.');
    }

    if (windowsPhoneTargetVersion === '8' || windowsPhoneTargetVersion === '8.0') {
        throw new CordovaError('8.0 is not a valid version for windows-phone-target-version (use the wp8 Cordova platform instead)');
    }
    var windowsReqVersion = Version.tryParse(REQUIRED_VERSIONS[windowsTargetVersion][requirement]);
    var phoneReqVersion = Version.tryParse(REQUIRED_VERSIONS[windowsPhoneTargetVersion][requirement]);

    // If we're searching for Windows SDK, we're not
    // interested in Phone's version and and vice versa.
    if (requirement === 'windowssdk') return windowsReqVersion;
    if (requirement === 'phonesdk') return phoneReqVersion;

    // If both windowsReqVersion and phoneReqVersion is valid Versions, choose the max one
    if (windowsReqVersion && phoneReqVersion) {
        return windowsReqVersion.gt(phoneReqVersion) ?
            windowsReqVersion :
            phoneReqVersion;
    }

    // Otherwise return that one which is defined and valid
    return windowsReqVersion || phoneReqVersion;
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

