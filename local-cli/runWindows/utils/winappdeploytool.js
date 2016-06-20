'use strict';

var fs = require('fs');
var path = require('path');
var execSync = require('child_process').execSync;
var chalk = require('chalk');

function DeviceInfo(deviceIndex, deviceName, deviceType) {
  this.index = deviceIndex;
  this.name = deviceName;
  this.type = deviceType;
}

DeviceInfo.prototype.toString = function() {
  return this.index + '. ' + this.name + ' (' + this.type + ')';
};

function WinAppDeployTool() {
  var programFilesPath = process.env['ProgramFiles(x86)'] || process.env['ProgramFiles'];
  this.path = path.join(programFilesPath, 'Windows Kits', '10', 'bin', 'x86', 'WinAppDeployCmd.exe');
  this.targetOSVersion = '10.0';
}

WinAppDeployTool.prototype.isAvailable = function () {
  return fs.existsSync(this.path);
};

function sortDevices (l, r) { return l.toString().length > r.toString().length; }

WinAppDeployTool.prototype.findDevice = function (target) {
  var devices = this.enumerateDevices();
  if (target === 'emulator') {
    var sortedList = devices.sort(sortDevices);
    for (var i = 0; i < sortedList.length; i++) {
      var sortedItem = sortedList[i];
      if (sortedItem.toString().indexOf(target) > -1) {
        return sortedItem;
      }
    }
  }

  if (target === 'device') {
    return devices[0];
  }

  var candidateList = devices.filter(function (device) {
    return device.__guid === target;
  });

  if (candidateList.length > 0) {
    return candidateList[0];
  }

  throw new Error('No devices found');
};

WinAppDeployTool.prototype.enumerateDevices = function () {
  // 127.0.0.1   00000015-b21e-0da9-0000-000000000000    Lumia 1520 (RM-940)\r
  //  maps to
  // [(line), '127.0.0.1', '00000015-b21e-0da9-0000-000000000000', 'Lumia 1520 (RM-940)']
  // The expansion is: IP address, spaces, GUID, spaces, text name
  var LINE_TEST = /^([\d\.]+?)\s+([\da-fA-F\-]+?)\s+(.+)$/m;

  var result = execSync('"' + this.path + '" devices').toString();
  var lines = result.split('\r\n');
  var matchedLines = lines.filter(function (line) {
    return LINE_TEST.test(line);
  });

  var devices = matchedLines.map(function (line, arrayIndex) {
    var match = line.match(LINE_TEST);
    var ip = match[1];
    var guid = match[2];
    var name = match[3];
    var type = 'device';

    var deviceInfo = new DeviceInfo(arrayIndex, name, type);
    deviceInfo.__ip = ip;
    deviceInfo.__guid = guid;

    return deviceInfo;
  });

  return devices;
};

WinAppDeployTool.prototype.uninstallAppPackage = function (packageInfo, targetDevice) {
  return execSync('"' + this.path + '" uninstall -package ' + packageInfo + ' -ip ' + targetDevice.__ip).toString();
};

WinAppDeployTool.prototype.installAppPackage = function (pathToAppxPackage, targetDevice, shouldLaunch, shouldUpdate, pin) {
  if (shouldLaunch) {
    console.log(chalk.yellow( 'Cannot launch app with current version of Windows 10 SDK tools. ' +
      'You will have to launch the app after installation is completed.'));
  }

  var args = [
    '"' + this.path + '"',
    shouldUpdate ? 'update' : 'install',
    '-file',
    pathToAppxPackage, '-ip',
    targetDevice.__ip
  ];

  if (pin) {
    args.push('-pin', pin);
  }

  return execSync(args.join(' ')).toString();
};

module.exports = WinAppDeployTool;
