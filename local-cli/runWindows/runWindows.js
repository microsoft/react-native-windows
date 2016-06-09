'use strict';

var path = require('path');
var child_process = require('child_process');
var chalk = require('chalk');
var glob = require('glob');
var MSBuildTools = require('./utils/msbuildtools');

/**
 * Starts the app on a connected Windows emulator or mobile device.
 */
function runWindows(options) {
  var slnFiles = getSolutionFile();
  if (slnFiles.length === 0) {
    console.log(chalk.red('Visual Studio Solution file not found. Maybe run rnpm windows first?'));
    return;
  }

  var slnFile = slnFile[0];

  // Validate args
  if (options.debug && options.release) {
    console.log(chalk.red('Only one of "debug"/"release" options should be specified'));
    return;
  }
  if (options.device && options.emulator) {
    console.log(chalk.red('Only one of "device"/"emulator"/"target" options should be specified'));
    return;
  }

  // Get build/deploy options
  var buildType = options.release ? 'release' : 'debug';
  var buildArchs = options.archs ? options.archs.split(' ') : ['anycpu'];
  var deployTarget = options.target ? options.target : (options.emulator ? 'emulator' : 'device');

  var msBuildTools = MSBuildTools.findAvailableVersion();

  // Restore the NuGet packages
  child_process.execSync('../.nuget/nuget.exe restore ' + slnFile + ' -NonInteractive');
}

function getSolutionFile() {
  return glob.sync(path.join(process.cwd(), 'windows/*.sln'));
}

module.exports = runWindows;
