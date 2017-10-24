'use strict';

const execSync = require('child_process').execSync;
const path = require('path');
const chalk = require('chalk');
const glob = require('glob');
const MSBuildTools = require('./msbuildtools');
const Version = require('./version');
const CONSTANTS = require('../constants')

function buildSolution(slnFile, buildType, buildArch, verbose, windowsOrWpf) {
  const minVersion = createMinVersion(windowsOrWpf);
  const allVersions = MSBuildTools.getAllAvailableVersions(windowsOrWpf);
  if (!allVersions.some(v => v.gte(minVersion))) {
    var errorString;
    if (windowsOrWpf === CONSTANTS.windows) {
      errorString = 'Must have a minimum Windows SDK version 10.0.10586.0 installed';
    } else {
      errorString = 'Must have a minimum Windows SDK version 8.1 installed';
    }
    throw new Error(errorString);
    
  }

  console.log(chalk.green(`Building ${slnFile}`));
  const msBuildTools = MSBuildTools.findAvailableVersion();
  msBuildTools.buildProject(slnFile, buildType, buildArch, null, verbose);

  function createMinVersion(windowsOrWpf) {
    if (windowsOrWpf === CONSTANTS.windows) {
      return new Version(10, 0, 10586, 0);
    } else {
      return new Version(8, 1, 0, 0);
    }
  }
}

function restoreNuGetPackages(options, slnFile, verbose, windowsOrWpf) {
  const runWhich = windowsOrWpf === CONSTANTS.windows? 'runWindows' : 'runWpf';
  let nugetPath = options.nugetPath || path.join(options.root, 'node_modules/react-native-windows/local-cli/' + runWhich + '/.nuget/nuget.exe');

  console.log(chalk.green('Restoring NuGet packages'));
  const verboseOption = verbose ? 'normal' : 'quiet';
  // Always inherit from stdio as we're controlling verbosity output above.
  execSync(`"${nugetPath}" restore "${slnFile}" -NonInteractive -Verbosity ${verboseOption}`, { stdio: 'inherit' });
}

function getSolutionFile(options, windowsOrWpf) {
  const platform = windowsOrWpf === CONSTANTS.windows? 'windows' : 'wpf';
  return glob.sync(path.join(options.root, platform + '/*.sln'))[0];
}

module.exports = {
  buildSolution,
  getSolutionFile,
  restoreNuGetPackages
};
