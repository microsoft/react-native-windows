'use strict';

const child_process = require('child_process');
const spawn = child_process.spawn, execSync = child_process.execSync;
const fs = require('fs');
const http = require('http');
const path = require('path');
const chalk = require('chalk');
const glob = require('glob');
const parse = require('xml-parser');
const WinAppDeployTool = require('./winappdeploytool');

function getAppPackage(options) {
  return glob.sync(path.join(options.root, 'windows/*/AppPackages/*'))[0];
}

function getWindowsStoreAppUtils(options) {
  const appStorePath = path.join(options.root, 'node_modules/react-native-windows/local-cli/runWindows/utils/WindowsStoreAppUtils.ps1');
  execSync(`powershell Unblock-File "${appStorePath}"`);
  return appStorePath;
}

function getAppxManifest(options) {
  const configuration = options.debug ? 'Debug' : 'Release';
  const appxPath = glob.sync(path.join(options.root, `windows/*/bin/${options.arch}/${configuration}/AppxManifest.xml`))[0];
  return parse(fs.readFileSync(appxPath, 'utf8'));
}

function handleResponseError(e) {
  if (e.message.indexOf('Error code -2146233088')) {
    throw new Error(`No Windows Mobile device was detected: ${e.message}`);
  } else {
    throw new Error(`Unexpected error deploying app: ${e.message}`);
  }
}

// Errors: 0x80073d10 - bad architecture
function deployToDevice(options) {
  const appPackageFolder = getAppPackage(options);

  const deployTarget = options.target ? options.target : (options.emulator ? 'emulator' : 'device');
  const deployTool = new WinAppDeployTool();
  const appxManifest = getAppxManifest(options);
  const identity = appxManifest.root.children.filter(function (x) { return x.name === 'mp:PhoneIdentity'; })[0];
  const appName = identity.attributes.PhoneProductId;

  return new Promise(resolve => {
    const device = deployTool.findDevice(deployTarget);

    try {
      deployTool.uninstallAppPackage(appName, device);
    } catch (e) {
      console.log(chalk.yellow('Failed to uninstall app from ' + device.name));
    }

    const appxFile = glob.sync(path.join(appPackageFolder, '*.appx'))[0];
    try {
      console.log(chalk.white(deployTool.installAppPackage(appxFile, device, true, false)));
      resolve();
    } catch (e) {
      if (e.message.indexOf('Error code 2148734208 for command') !== -1) {
        console.log(chalk.white(deployTool.installAppPackage(appxFile, device, true, true)));
        resolve();
      } else {
        handleResponseError(e);
      }
    }
  });
}

function deployToDesktop(options) {
  const appPackageFolder = getAppPackage(options);
  const windowsStoreAppUtils = getWindowsStoreAppUtils(options);
  const appxManifest = getAppxManifest(options);
  const identity = appxManifest.root.children.filter(function (x) { return x.name === 'Identity'; })[0];
  const appName = identity.attributes.Name;
  const script = glob.sync(path.join(appPackageFolder, 'Add-AppDevPackage.ps1'))[0];


  return new Promise(resolve => {
    console.log(chalk.green('Removing old version of the app'));
    execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}" ; Uninstall-App ${appName}`);

    console.log(chalk.green('Installing new version of the app'));
    execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}"; Install-App "${script}"`);

    console.log(chalk.green('Starting the app'));
    execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}"; Start-Locally ${appName}`);
    resolve();
  });
}

function startServerInNewWindow(options) {
  return new Promise(resolve => {
    http.get('http://localhost:8081/status', res => {
      if (res === 200) {
        console.log(chalk.green('React-Native Server already started'));
        resolve();
      }
    }).on('error', () => resolve(launchServer(options)));
  });
}

function launchServer(options) {
  console.log(chalk.green('Starting the React-Native Server'));
  const launchPackagerScript = path.join('node_modules/react-native/packager/launchPackager.bat');
  const opts = {
    cwd: options.root,
    detached: true,
    stdio: 'ignore'
  };

  return Promise.resolve(spawn('cmd.exe', ['/C', 'start', launchPackagerScript], opts));
}

module.exports = {
  deployToDesktop,
  deployToDevice,
  startServerInNewWindow
};
