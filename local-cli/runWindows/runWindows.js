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
  if (options.debug && options.release) {
    console.log(chalk.red('Only one of "debug"/"release" options should be specified'));
    return;
  }
  if (options.device && options.emulator) {
    console.log(chalk.red('Only one of "device"/"emulator"/"target" options should be specified'));
    return;
  }

  var slnFile = getSolutionFile(options);
  if (!slnFile) {
    console.log(chalk.red('Visual Studio Solution file not found. Maybe run rnpm windows first?'));
    return;
  }

  try {
    cleanSolution(options);
  } catch (e) {
    console.log(chalk.red('Failed to remove the AppPackages folder'));
  }

  try {
    restoreNuGetPackages(options, slnFile);
  } catch (e) {
    console.log(chalk.red('Failed to restore the NuGet packages'));
    return;
  }

  // Get build/deploy options
  var buildType = options.release ? 'release' : 'Debug';
  var buildArch = options.arch ? options.arch : 'anycpu';
  var deployTarget = options.target ? options.target : (options.emulator ? 'emulator' : 'device');

  try {
    buildSolution(slnFile, buildType, buildArch);
  } catch (e) {
    console.log(chalk.red('Build failed with message ' + e + '. Check your build configuration.'));
    return;
  }

  try {
    startServerInNewWindow(options);
  } catch (e) {
    console.log(chalk.red('Failed to start the development server'));
    return;
  }

  try {
    deploySolution(options, deployTarget);
  } catch (e) {
    console.log(chalk.red('Failed to deploy the application'));
    return;
  }
}

function cleanSolution(options) {
  shell.rm('-rf', glob.sync(path.join(options.root, 'windows/*/AppPackages')));
}

function buildSolution(slnFile, buildType, buildArch) {
  console.log(chalk.green('Building the solution'));
  var msBuildTools = MSBuildTools.findAvailableVersion();
  msBuildTools.buildProject(slnFile, buildType, buildArch, null);
}

function restoreNuGetPackages(options, slnFile) {
  console.log(chalk.green('Restoring NuGet packages'));
  var nugetPath = options.nugetPath || '../.nuget/nuget.exe';
  var results = child_process.execSync(nugetPath + ' restore ' + slnFile + ' -NonInteractive').toString().split('\r\n');
  results.forEach(function (result) { console.log(chalk.green(result)); });
}

function startServerInNewWindow(options) {
  console.log(chalk.green('Starting the React-Native Server'));
  var launchPackagerScript = path.resolve('node_modules/react-native/packager/launchPackager.bat');
  var options = {
    cwd: options.root,
    detached: true,
    stdio: 'ignore'
  };

  child_process.spawn('cmd.exe', ['/C', 'start', launchPackagerScript], options);
}

function deploySolution(options) {
  var appPackageFolder = getAppPackage(options);

  var windowsStoreAppUtils = getWindowsStoreAppUtils();
  var appxManifest = getAppxManifest(options);
  var identity = appxManifest.root.children.filter(function (x) { return x.name === 'Identity'; })[0];
  var appName = identity.attributes.Name;

  console.log(chalk.green('Removing old version of the app'));
  child_process.execSync('powershell -ExecutionPolicy RemoteSigned Import-Module "' +
    windowsStoreAppUtils + '" ; Uninstall-App ' + appName);

  console.log(chalk.green('Installing new version of the app'));
  var script = glob.sync(path.join(appPackageFolder, 'Add-AppDevPackage.ps1'))[0];
  child_process.execSync('powershell -ExecutionPolicy RemoteSigned Import-Module "' +
    windowsStoreAppUtils + '" ; Install-App "' + script + '"');

  console.log(chalk.green('Starting the app'));
  child_process.execSync('powershell -ExecutionPolicy RemoteSigned Import-Module "' +
    windowsStoreAppUtils + '" ; Start-Locally ' + appName);
}

function getSolutionFile(options) {
  return glob.sync(path.join(options.root, 'windows/*.sln'))[0];
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
// Example of running the Windows Command
runWindows({
  root: 'C:\\github\\hack\\myapp',
  debug: true,
  arch: 'x86',
  nugetPath: 'C:\\github\\react\\react-native-windows\\local-cli\\runWindows\\.nuget\\nuget.exe',
});
*/
