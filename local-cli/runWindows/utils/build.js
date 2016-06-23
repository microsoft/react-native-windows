'use strict';

const EOL = require('os').EOL;
const execSync = require('child_process').execSync;
const path = require('path');
const chalk = require('chalk');
const glob = require('glob');
const shell = require('shelljs');
const MSBuildTools = require('./msbuildtools');
const Version = require('./version');

function cleanSolution(options) {
  const appPackagesPath = glob.sync(path.join(options.root, 'windows/*/AppPackages'));
  console.log(chalk.green(`Cleaning AppPackages folder ${appPackagesPath}`));
  shell.rm('-rf', appPackagesPath);
}

function buildSolution(slnFile, buildType, buildArch) {
  const minVersion = new Version(10, 0, 10586, 0);
  const allVersions = MSBuildTools.getAllAvailableUAPVersions();
  if (!allVersions.some(v => v.gte(minVersion))) {
    throw new Error('Must have a minimum Windows SDK version 10.0.10586.0 installed');
  }

  console.log(chalk.green(`Building ${slnFile}`));
  const msBuildTools = MSBuildTools.findAvailableVersion();
  msBuildTools.buildProject(slnFile, buildType, buildArch, null);
}

function restoreNuGetPackages(options, slnFile) {
  console.log(chalk.green('Restoring NuGet packages'));
  const nugetPath = options.nugetPath || path.join(options.root, 'node_modules/react-native-windows/local-cli/runWindows/.nuget/nuget.exe');
  const results = execSync(`"${nugetPath}" restore "${slnFile}" -NonInteractive`).toString().split(EOL);
  results.forEach(result => console.log(chalk.white(result)));
}

function getSolutionFile(options) {
  return glob.sync(path.join(options.root, 'windows/*.sln'))[0];
}

module.exports = {
  buildSolution,
  cleanSolution,
  getSolutionFile,
  restoreNuGetPackages
};
