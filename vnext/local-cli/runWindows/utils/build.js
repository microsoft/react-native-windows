/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check
'use strict';

const fs = require('fs');
const os = require('os');
const path = require('path');
const glob = require('glob');
const MSBuildTools = require('./msbuildtools');
const Version = require('./version');
const {commandWithProgress, newSpinner} = require('./commandWithProgress');
const util = require('util');
const existsAsync = util.promisify(fs.exists);

async function buildSolution(slnFile, buildType, buildArch, verbose) {
  const minVersion = new Version(10, 0, 18362, 0);
  const allVersions = MSBuildTools.getAllAvailableUAPVersions();
  if (!allVersions.some(v => v.gte(minVersion))) {
    throw new Error(
      'Must have a minimum Windows SDK version 10.0.18362.0 installed',
    );
  }

  const msBuildTools = MSBuildTools.findAvailableVersion(buildArch, verbose);
  await msBuildTools.buildProject(slnFile, buildType, buildArch, null, verbose);
}

async function nugetRestore(nugetPath, slnFile, verbose, msbuildVersion) {
  const text = 'Restoring NuGets';
  const spinner = newSpinner(text);
  console.log(nugetPath);
  await commandWithProgress(
    spinner,
    text,
    nugetPath,
    [
      'restore',
      `${slnFile}`,
      '-NonInteractive',
      '-Verbosity',
      verbose ? 'normal' : 'quiet',
      '-MSBuildVersion',
      msbuildVersion,
    ],
    verbose,
  );
}

async function restoreNuGetPackages(options, slnFile, verbose) {
  const nugetPath =
    options.nugetPath || path.join(os.tmpdir(), 'nuget.4.9.2.exe');

  const dlNugetText = 'Downloading NuGet Binary';
  const ensureNugetSpinner = newSpinner(dlNugetText);
  const exists = await existsAsync(nugetPath);
  if (!exists) {
    await commandWithProgress(
      ensureNugetSpinner,
      dlNugetText,
      'powershell',
      `Invoke-WebRequest https://dist.nuget.org/win-x86-commandline/v4.9.2/nuget.exe -outfile ${nugetPath}`.split(
        ' ',
      ),
      verbose,
    );
  }
  ensureNugetSpinner.succeed('Found NuGet Binary');

  const msbuildTools = MSBuildTools.findAvailableVersion('x86', verbose);
  await nugetRestore(
    nugetPath,
    slnFile,
    verbose,
    msbuildTools.installationVersion,
  );
}

function getSolutionFile(options) {
  return glob.sync(path.join(options.root, 'windows/*.sln'))[0];
}

module.exports = {
  buildSolution,
  getSolutionFile,
  restoreNuGetPackages,
};
