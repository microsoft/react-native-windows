'use strict';

const EOL = require('os').EOL;
const execSync = require('child_process').execSync;
const path = require('path');
const chalk = require('chalk');
const glob = require('glob');
const shell = require('shelljs');
const MSBuildTools = require('./msbuildtools');
const Version = require('./version');

function buildSolution(slnFile, buildType, buildArch) {
  const minVersion = new Version(8, 1, 0, 0);
  const allVersions = MSBuildTools.getAllAvailableVersions();
  if (!allVersions.some(v => v.gte(minVersion))) {
    throw new Error('Must have a minimum Windows SDK version 8.1 installed');
  }

  console.log(chalk.green(`Building ${slnFile}`));
  const msBuildTools = MSBuildTools.findAvailableVersion();
  msBuildTools.buildProject(slnFile, buildType, buildArch, null);
}

function restoreNuGetPackages(options, slnFile) {
  console.log(chalk.green('Restoring NuGet packages'));
  const nugetPath = options.nugetPath || path.join(options.root, 'node_modules/react-native-windows/local-cli/runWpf/.nuget/nuget.exe');
  const results = execSync(`"${nugetPath}" restore "${slnFile}" -NonInteractive`).toString().split(EOL);
  results.forEach(result => console.log(chalk.white(result)));
}

function getSolutionFile(options) {
  return glob.sync(path.join(options.root, 'wpf/*.sln'))[0];
}

module.exports = {
  buildSolution,
  getSolutionFile,
  restoreNuGetPackages
};
