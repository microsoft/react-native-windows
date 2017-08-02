'use strict';

const EOL = require('os').EOL;
const execSync = require('child_process').execSync;
const path = require('path');
const chalk = require('chalk');
const glob = require('glob');
const shell = require('shelljs');
const MSBuildTools = require('./msbuildtools');
const Version = require('./version');

function buildSolution(slnFile, buildType, buildArch, verbose) {
  const minVersion = new Version(8, 1, 0, 0);
  const allVersions = MSBuildTools.getAllAvailableVersions();
  if (!allVersions.some(v => v.gte(minVersion))) {
    throw new Error('Must have a minimum Windows SDK version 8.1 installed');
  }

  console.log(chalk.green(`Building ${slnFile}`));
  const msBuildTools = MSBuildTools.findAvailableVersion();
  msBuildTools.buildProject(slnFile, buildType, buildArch, null, verbose);
}

function restoreNuGetPackages(options, slnFile) {
  let nugetPath = options.nugetPath || path.join(options.root, 'node_modules/react-native-windows/local-cli/runWpf/.nuget/nuget.exe');
  const msBuildTools = MSBuildTools.findAvailableVersion();
  if(msBuildTools.version === '15.0' && !options.nugetPath) {
    console.log(chalk.green('Using NuGet version >v4.*'));
    nugetPath = path.join(options.root, 'node_modules/react-native-windows/local-cli/runWpf/.nuget/nuget.v4.1.0.exe');
  }

  console.log(chalk.green('Restoring NuGet packages'));
  const verboseOption = options.verbose ? 'normal' : 'quiet';
  // Always inherit from stdio as we're controlling verbosity output above.
  execSync(`"${nugetPath}" restore "${slnFile}" -NonInteractive -Verbosity ${verboseOption}`, { stdio: 'inherit' });
}

function getSolutionFile(options) {
  return glob.sync(path.join(options.root, 'wpf/*.sln'))[0];
}

module.exports = {
  buildSolution,
  getSolutionFile,
  restoreNuGetPackages
};
