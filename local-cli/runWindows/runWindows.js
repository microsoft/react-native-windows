'use strict';

var execSync = require('child_process').execSync;
var spawn = require('child_process').spawn;
var fs = require('fs');
var path = require('path');
var chalk = require('chalk');
var glob = require('glob');
var parse = require('xml-parser');
var shell = require('shelljs');
var MSBuildTools = require('./utils/msbuildtools');
var WinAppDeployTool = require('./utils/winappdeploytool');

/**
 * Starts the app on a connected Windows emulator or mobile device.
 * Options are the following:
 *    root: String - The root of the application
 *    debug: Boolean - Specifies debug build
 *    release: Boolean - Specifies release build
 *    arch: String - The build architecture (x86, x64, ARM, Any CPU)
 *    desktop: Boolean - Deploy to the desktop
 *    emulator: Boolean - Deploy to the emulator
 *    device: Boolean - Deploy to a device
 *    target: String - Device GUID to deploy to
 */
function runWindows(options) {
  // Fix up options
  options.root = options.root || process.cwd();
  if (options.debug && options.release) {
    console.log(chalk.red('Only one of "debug"/"release" options should be specified'));
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
  var buildType = options.release ? 'Release' : 'Debug';
  var buildArch = options.arch ? options.arch : 'anycpu';

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

  if (options.desktop) {
    try {
      deployToDesktop(options);
    } catch (e) {
      console.log(chalk.red('Failed to deploy the application'));
      return;
    }
  }

  if (options.device || options.emulator || options.target) {
    try {
      deployToDevice(options);
    } catch (e) {
      console.log(chalk.red('Failed to deploy the application to the device or emulator'));
    }
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
  var results = execSync(nugetPath + ' restore ' + slnFile + ' -NonInteractive').toString().split('\r\n');
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

  spawn('cmd.exe', ['/C', 'start', launchPackagerScript], options);
}

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
    deployTool.installAppPackage(appxFile, device, true, false);
  } catch (e) {
    if (e.message.indexOf('Error code 2148734208 for command') !== -1) {
      try {
        deployTool.installAppPackage(appxFile, device, true, true);
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

function getSolutionFile(options) {
  return glob.sync(path.join(options.root, 'windows/*.sln'))[0];
}

function getAppPackage(options) {
  return glob.sync(path.join(options.root, 'windows/*/AppPackages/*'))[0];
}

function getWindowsStoreAppUtils() {
  var appStorePath = path.join(__dirname, 'utils/WIndowsStoreAppUtils.ps1');
  execSync('powershell Unblock-File ' + appStorePath);
  return appStorePath;
}

function getAppxManifest(options) {
  var appxPath = glob.sync(path.join(options.root, 'windows/*/bin/*/*/AppxManifest.xml'))[0];
  return parse(fs.readFileSync(appxPath, 'utf8'));
}

module.exports = runWindows;

// Example of running the Windows Command
runWindows({
  root: 'C:\\github\\hack\\myapp',
  debug: true,
  arch: 'x86',
  nugetPath: 'C:\\github\\react\\react-native-windows\\local-cli\\runWindows\\.nuget\\nuget.exe',
  device: true,
});

