'use strict';

const execSync = require('child_process').execSync;
const path = require('path');
const chalk = require('chalk');
const glob = require('glob');
const shell = require('shelljs');
const MSBuildTools = require('./msbuildtools');

function cleanSolution(options) {
  const appPackagesPath = glob.sync(path.join(options.root, 'windows/*/AppPackages'));
  console.log(chalk.green(`Cleaning AppPackages folder ${appPackagesPath}`));
  shell.rm('-rf', appPackagesPath);
}

function buildSolution(slnFile, buildType, buildArch) {
  console.log(chalk.green(`Building ${slnFile}`));
  const msBuildTools = MSBuildTools.findAvailableVersion();
  msBuildTools.buildProject(slnFile, buildType, buildArch, null);
}

function restoreNuGetPackages(options, slnFile) {
  console.log(chalk.green('Restoring NuGet packages'));
  const nugetPath = options.nugetPath || path.resolve('../.nuget/nuget.exe');
  const results = execSync(`"${nugetPath}" restore "${slnFile}" -NonInteractive`).toString().split('\r\n');
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
