'use strict';

var child_process = require('child_process');
var execSync = child_process.execSync, spawn = child_process.spawn;
var fs = require('fs');
var http = require('http');
var path = require('path');
var chalk = require('chalk');
var glob = require('glob');
var parse = require('xml-parser');
var WinAppDeployTool = require('./winappdeploytool');

function getAppPackage(options) {
  return glob.sync(path.join(options.root, 'windows/*/AppPackages/*'))[0];
}

function getWindowsStoreAppUtils() {
  var appStorePath = path.join(__dirname, 'WindowsStoreAppUtils.ps1');
  execSync('powershell Unblock-File ' + appStorePath);
  return appStorePath;
}

function getAppxManifest(options) {
  var appxPath = glob.sync(path.join(options.root, 'windows/*/bin/*/*/AppxManifest.xml'))[0];
  return parse(fs.readFileSync(appxPath, 'utf8'));
}

// Errors: 0x80073d10 - bad architecture
function deployToDevice(options) {
  var appPackageFolder = getAppPackage(options);

  var deployTarget = options.target ? options.target : (options.emulator ? 'emulator' : 'device');
  var deployTool = new WinAppDeployTool();
  var appxManifest = getAppxManifest(options);
  var identity = appxManifest.root.children.filter(function (x) { return x.name === 'mp:PhoneIdentity'; })[0];
  var appName = identity.attributes.PhoneProductId;

  var device;
  try {
    device = deployTool.findDevice(deployTarget);
  } catch (e) {
    console.log(chalk.red(e.message));
    return;
  }

  try {
    console.log(chalk.green('Uninstalling app from ' + device.name))
    deployTool.uninstallAppPackage(appName, device);
  } catch (e) {
    console.log(chalk.yellow('Failed to uninstall app from ' + device.name));
  }

  var appxFile = glob.sync(path.join(appPackageFolder, '*.appx'))[0];
  console.log('appxFile', appxFile);
  try {
    console.log(chalk.green('Installing app to ' + device.name));
    console.log(chalk.green(deployTool.installAppPackage(appxFile, device, true, false)));
  } catch (e) {
    if (e.message.indexOf('Error code 2148734208 for command') !== -1) {
      try {
        console.log(chalk.green(deployTool.installAppPackage(appxFile, device, true, true)));
      } catch (err) {
        console.log(chalk.red('Unexpected error deploying app: ' + e.message));
        return;
      }
    } else if (e.message.indexOf('Error code -2146233088')) {
      console.log(chalk.red('No Windows Mobile device was detected.'));
      return;
    } else {
      console.log(chalk.red('Unexpected error deploying app: ' + e.message));
      return;
    }
  }
}

function deployToDesktop(options) {
  var appPackageFolder = getAppPackage(options);

  var windowsStoreAppUtils = getWindowsStoreAppUtils();
  var appxManifest = getAppxManifest(options);
  var identity = appxManifest.root.children.filter(function (x) { return x.name === 'Identity'; })[0];
  var appName = identity.attributes.Name;

  console.log(chalk.green('Removing old version of the app'));
  execSync('powershell -ExecutionPolicy RemoteSigned Import-Module "' +
    windowsStoreAppUtils + '" ; Uninstall-App ' + appName);

  console.log(chalk.green('Installing new version of the app'));
  var script = glob.sync(path.join(appPackageFolder, 'Add-AppDevPackage.ps1'))[0];
  execSync('powershell -ExecutionPolicy RemoteSigned Import-Module "' +
    windowsStoreAppUtils + '" ; Install-App "' + script + '"');

  console.log(chalk.green('Starting the app'));
  execSync('powershell -ExecutionPolicy RemoteSigned Import-Module "' +
    windowsStoreAppUtils + '" ; Start-Locally ' + appName);
}

function startServerInNewWindow(options) {
  http.get('http://localhost:8081/status', function (res) {
    if (res === 200) {
      console.log(chalk.green('React-Native Server already started'));
    } else {
      launchServer(options);
    }
  }).on('error', function () { launchServer(options); });
}

function launchServer(options) {
  console.log(chalk.green('Starting the React-Native Server'));
  var launchPackagerScript = path.resolve('node_modules/react-native/packager/launchPackager.bat');
  var opts = {
    cwd: options.root,
    detached: true,
    stdio: 'ignore'
  };

  spawn('cmd.exe', ['/C', 'start', launchPackagerScript], opts);
}

module.exports = {
  deployToDesktop,
  deployToDevice,
  startServerInNewWindow
};
