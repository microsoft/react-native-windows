'use strict';

var chalk = require('chalk');
var build = require('./utils/build');
var deploy = require('./utils/deploy');

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

  var slnFile = build.getSolutionFile(options);
  if (!slnFile) {
    console.log(chalk.red('Visual Studio Solution file not found. Maybe run rnpm windows first?'));
    return;
  }

  try {
    build.cleanSolution(options);
  } catch (e) {
    console.log(chalk.red('Failed to remove the AppPackages folder'));
  }

  try {
    build.restoreNuGetPackages(options, slnFile);
  } catch (e) {
    console.log(chalk.red('Failed to restore the NuGet packages'));
    return;
  }

  // Get build/deploy options
  var buildType = options.release ? 'Release' : 'Debug';
  var buildArch = options.arch ? options.arch : 'anycpu';

  try {
    build.buildSolution(slnFile, buildType, buildArch);
  } catch (e) {
    console.log(chalk.red('Build failed with message ' + e + '. Check your build configuration.'));
    return;
  }

  try {
    deploy.startServerInNewWindow(options);
  } catch (e) {
    console.log(chalk.red('Failed to start the development server'));
    return;
  }

  if (options.desktop) {
    try {
      deploy.deployToDesktop(options);
    } catch (e) {
      console.log(chalk.red('Failed to deploy the application'));
      return;
    }
  }

  if (options.device || options.emulator || options.target) {
    try {
      deploy.deployToDevice(options);
    } catch (e) {
      console.log(chalk.red('Failed to deploy the application to the device or emulator'));
    }
  }
}

module.exports = runWindows;

/**
// Example of running the Windows Command
runWindows({
  root: 'C:\\github\\hack\\myapp',
  debug: true,
  arch: 'ARM',
  nugetPath: 'C:\\github\\react\\react-native-windows\\local-cli\\runWindows\\.nuget\\nuget.exe',
  device: true,
});
*/
