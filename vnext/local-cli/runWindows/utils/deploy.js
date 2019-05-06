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

function pushd(pathArg) {
  const cwd = process.cwd();
  process.chdir(pathArg);
  return () => process.chdir(cwd);
}

function getAppPackage(options) {
  const configuration = options.release ? 'Release' : 'Debug';
  return glob.sync(`windows/*/AppPackages/*_${options.arch}_${configuration}_*`)[0];
}

function getWindowsStoreAppUtils(options) {
  const popd = pushd(options.root);
  const windowsStoreAppUtilsPath = './node_modules/react-native-windows/local-cli/runWindows/utils/WindowsStoreAppUtils.ps1';
  execSync(`powershell Unblock-File "${windowsStoreAppUtilsPath}"`);
  popd();
  return windowsStoreAppUtilsPath;
}

function getAppxManifest(options) {
  const configuration = options.release ? 'Release' : 'Debug';
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
      deployTool.uninstallAppPackage(appName, device, options.verbose);
    } catch (e) {
      console.log(chalk.yellow('Failed to uninstall app from ' + device.name));
    }

    const appxFile = glob.sync(path.join(appPackageFolder, '*.appx'))[0];
    try {
      console.log(chalk.white(deployTool.installAppPackage(appxFile, device, true, false, options.verbose)));
      resolve();
    } catch (e) {
      if (e.message.indexOf('Error code 2148734208 for command') !== -1) {
        console.log(chalk.white(deployTool.installAppPackage(appxFile, device, true, true, options.verbose)));
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
  const args = ['remoteDebugging', options.proxy ? 'true' : 'false'];
  const execOptions = options.verbose ? { stdio: 'inherit' } : {};

  return new Promise(resolve => {
    const popd = pushd(options.root);

    console.log(chalk.green('Removing old version of the app'));
    execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}" ; Uninstall-App ${appName}`, execOptions);

    console.log(chalk.green('Installing new version of the app'));
    execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}"; Install-App "${script}"`, execOptions);

    const appFamilyName = execSync(`powershell -c $(Get-AppxPackage -Name ${appName}).PackageFamilyName`).toString().trim();
    execSync(`CheckNetIsolation LoopbackExempt -a -n=${appFamilyName}`, execOptions);

    console.log(chalk.green('Starting the app'));
    execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}"; Start-Locally ${appName} ${args}`, execOptions);

    popd();

    resolve();
  });
}

function startServerInNewWindow(options) {
  return new Promise(resolve => {
    if (options.packager) {
      http.get('http://localhost:8081/status', res => {
        if (res.statusCode === 200) {
          console.log(chalk.green('React-Native Server already started'));
        } else {
          console.log(chalk.red('React-Native Server not responding'));
        }
        resolve();
      }).on('error', () => resolve(launchServer(options)));
    } else {
      resolve();
    }
  });
}

function launchServer(options) {
  console.log(chalk.green('Starting the React-Native Server'));
  const launchPackagerScript = path.join('node_modules/react-native-windows/Scripts/launchPackager.bat');
  const opts = {
    cwd: options.root,
    detached: true,
    stdio: options.verbose ? 'inherit' : 'ignore'
  };

  return Promise.resolve(spawn('cmd.exe', ['/C', 'start', launchPackagerScript], opts));
}

module.exports = {
  deployToDesktop,
  deployToDevice,
  startServerInNewWindow
};
