'use strict';

var child_process = require('child_process');
var fs = require('fs');
var path = require('path');
var chalk = require('chalk');
var glob = require('glob');
var parse = require('xml-parser');
var shell = require('shelljs');
var MSBuildTools = require('./utils/msbuildtools');

/**
 * Starts the app on a connected Windows emulator or mobile device.
 */
function runWindows(options) {
  // Fix up options
  options.root = options.root || process.cwd();

  var slnFiles = getSolutionFile(options);
  if (slnFiles.length === 0) {
    console.log(chalk.red('Visual Studio Solution file not found. Maybe run rnpm windows first?'));
    return;
  }

  var slnFile = slnFiles[0];

  // Validate args
  if (options.debug && options.release) {
    console.log(chalk.red('Only one of "debug"/"release" options should be specified'));
    return;
  }
  if (options.device && options.emulator) {
    console.log(chalk.red('Only one of "device"/"emulator"/"target" options should be specified'));
    return;
  }

  try {
    shell.rm('-rf', glob.sync(path.join(options.root, 'windows/*/AppPackages')));
  } catch (e) {
    console.log(chalk.red('Failed to remove the AppPackages folder'));
  }

  try {
    // Restore the NuGet packages
    var nugetPath = options.nugetPath || '../.nuget/nuget.exe';
    child_process.execSync(nugetPath + ' restore ' + slnFile + ' -NonInteractive');
  } catch (e) {
    console.log(chalk.red('Failed to restore the NuGet packages'));
    return;
  }

  // Get build/deploy options
  var buildType = options.release ? 'release' : 'debug';
  var buildArch = options.arch ? options.arch : 'anycpu';
  var deployTarget = options.target ? options.target : (options.emulator ? 'emulator' : 'device');

  var msBuildTools = MSBuildTools.findAvailableVersion();
  msBuildTools.buildProject(slnFile, buildType, buildArch, null);

  // Launch start
  var rnStart = child_process.spawn('react-native start', { cwd: options.root });
  rnStart.on('data', function (data) {
    console.log(chalk.yellow(data.toString()));
  });
  rnStart.on('error', function (err) {
    console.log(chalk.red(err.toString()));
  });

  var appPackageFolder = getAppPackage(options);

  var windowsStoreAppUtils = getWindowsStoreAppUtils();
  var appxManifest = getAppxManifest(options);
  var identity = appxManifest.root.children.filter(function (x) { return x.name === 'Identity'; })[0];
  var appName = identity.attributes.Name;
  child_process.execSync('powershell -ExecutionPolicy RemoteSigned Import-Module "' +
    windowsStoreAppUtils + '" ; Uninstall-App ' + appName);

  var script = glob.sync(path.join(appPackageFolder, 'Add-AppDevPackage.ps1'))[0];
  child_process.execSync('powershell -ExecutionPolicy RemoteSigned Import-Module "' +
    windowsStoreAppUtils + '" ; Install-App "' + script + '"');

  child_process.execSync('powershell -ExecutionPolicy RemoteSigned Import-Module "' +
    windowsStoreAppUtils + '" ; Start-Locally ' + appName);
}

function getSolutionFile(options) {
  return glob.sync(path.join(options.root, 'windows/*.sln'));
}

function getAppPackage(options) {
  return glob.sync(path.join(options.root, 'windows/*/AppPackages/*'))[0];
}

function getWindowsStoreAppUtils() {
  var appStorePath = path.join(__dirname, 'utils/WIndowsStoreAppUtils.ps1');
  child_process.execSync('powershell Unblock-File ' + appStorePath);
  return appStorePath;
}

function getAppxManifest(options) {
  var appxPath = glob.sync(path.join(options.root, 'windows/*/bin/*/*/AppxManifest.xml'))[0];
  return parse(fs.readFileSync(appxPath, 'utf8'));
}

module.exports = runWindows;

/*
runWindows({
  root: 'C:\\github\\hack\\myapp',
  debug: true,
  arch: 'x86',
  nugetPath: 'C:\\github\\react\\react-native-windows\\local-cli\\runWindows\\.nuget\\nuget.exe',
});
*/

