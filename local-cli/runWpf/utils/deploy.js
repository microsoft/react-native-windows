'use strict';

const child_process = require('child_process');
const spawn = child_process.spawn, execSync = child_process.execSync;
const fs = require('fs');
const http = require('http');
const path = require('path');
const chalk = require('chalk');
const glob = require('glob');
const parse = require('xml-parser');

function getAppPackage(options) {
  const configuration = options.release ? 'Release' : 'Debug';
  return glob.sync(path.join(options.root, `windows/*/AppPackages/*_${options.arch}_${configuration}_*`))[0];
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

function deployToDesktop(options) {
  const appPackageFolder = getAppPackage(options);
  const windowsStoreAppUtils = getWindowsStoreAppUtils(options);
  const appxManifest = getAppxManifest(options);
  const identity = appxManifest.root.children.filter(function (x) { return x.name === 'Identity'; })[0];
  const appName = identity.attributes.Name;
  const script = glob.sync(path.join(appPackageFolder, 'Add-AppDevPackage.ps1'))[0];
  const args = ["remoteDebugging", options.proxy ? 'true' : 'false'];

  return new Promise(resolve => {
    console.log(chalk.green('Removing old version of the app'));
    execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}" ; Uninstall-App ${appName}`);

    console.log(chalk.green('Installing new version of the app'));
    execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}"; Install-App "${script}"`);

    const appFamilyName = execSync(`powershell -c $(Get-AppxPackage -Name ${appName}).PackageFamilyName`);
    execSync(`CheckNetIsolation LoopbackExempt -a -n=${appFamilyName}`);

    console.log(chalk.green('Starting the app'));
    execSync(`powershell -ExecutionPolicy RemoteSigned Import-Module "${windowsStoreAppUtils}"; Start-Locally ${appName} ${args}`);
    resolve();
  });
}

function startServerInNewWindow(options) {
  return new Promise(resolve => {
    http.get('http://localhost:8081/status', res => {
      if (res.statusCode === 200) {
        console.log(chalk.green('React-Native Server already started'));
      } else {
        console.log(chalk.red('React-Native Server not responding'));
      }
      resolve();
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
  startServerInNewWindow
};
