'use strict';

const execSync = require('child_process').execSync;
const path = require('path');
const chalk = require('chalk');
const glob = require('glob');
const MSBuildTools = require('./msbuildtools');
const Version = require('./version');

function buildSolution(slnFile, buildType, buildArch, verbose) {
  const minVersion = new Version(10, 0, 10586, 0);
  const allVersions = MSBuildTools.getAllAvailableUAPVersions();
  if (!allVersions.some(v => v.gte(minVersion))) {
    throw new Error('Must have a minimum Windows SDK version 10.0.10586.0 installed');
  }

  console.log(chalk.green(`Building ${slnFile}`));
  const msBuildTools = MSBuildTools.findAvailableVersion();
  msBuildTools.buildProject(slnFile, buildType, buildArch, null, verbose);
}

function restoreNuGetPackages(options, slnFile, verbose) {
  let nugetPath = options.nugetPath || path.join(options.root, 'node_modules/react-native-windows/local-cli/runWindows/.nuget/nuget.exe');

  console.log(chalk.green('Restoring NuGet packages'));
  const verboseOption = verbose ? 'normal' : 'quiet';
  // Always inherit from stdio as we're controlling verbosity output above.
  execSync(`"${nugetPath}" restore "${slnFile}" -NonInteractive -Verbosity ${verboseOption}`, { stdio: 'inherit' });
}

function getSolutionFile(options) {
  return glob.sync(path.join(options.root, 'windows/*.sln'))[0];
}

module.exports = {
  buildSolution,
  getSolutionFile,
  restoreNuGetPackages,
};
