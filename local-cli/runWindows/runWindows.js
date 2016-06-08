'use strict';

var glob = require('glob');
var path = require('path');
var chalk = require('chalk');
var child_process = require('child_process');

/**
 * Starts the app on a connected Windows emulator or mobile device.
 */
function runWindows(args) {
  var slnFiles = getSolutionFile();
  if (slnFiles.length === 0) {
    console.log(chalk.red('Visual Studio Solution file not found. Maybe run rnpm windows first?'));
    return;
  }

  var slnFile = slnFile[0];

  // Restore the NuGet packages
  child_process.execSync('../.nuget/nuget.exe restore ' + slnFile + ' -NonInteractive');
}

function getSolutionFile() {
  return glob.sync(path.join(process.cwd(), 'windows/*.sln'));
}

module.exports = runWindows;
