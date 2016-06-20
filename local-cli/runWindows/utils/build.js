'use strict';

var execSync = require('child_process').execSync;
var path = require('path');
var chalk = require('chalk');
var glob = require('glob');
var shell = require('shelljs');
var MSBuildTools = require('./msbuildtools');

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

function getSolutionFile(options) {
  return glob.sync(path.join(options.root, 'windows/*.sln'))[0];
}

module.exports = {
  buildSolution: buildSolution,
  cleanSolution: cleanSolution,
  getSolutionFile: getSolutionFile,
  restoreNuGetPackages: restoreNuGetPackages
};
