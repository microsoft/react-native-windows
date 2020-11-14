/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check
'use strict';

const fs = require('fs');
const https = require('https');
const os = require('os');
const path = require('path');
const glob = require('glob');
const MSBuildTools = require('./msbuildtools');
const Version = require('./version');
const {commandWithProgress, newSpinner} = require('./commandWithProgress');
const util = require('util');
const chalk = require('chalk');
const existsAsync = util.promisify(fs.exists);

async function buildSolution(
  slnFile,
  buildType,
  buildArch,
  msBuildProps,
  verbose,
) {
  const minVersion = new Version(10, 0, 18362, 0);
  const allVersions = MSBuildTools.getAllAvailableUAPVersions();
  if (!allVersions.some(v => v.gte(minVersion))) {
    throw new Error(
      'Must have a minimum Windows SDK version 10.0.18362.0 installed',
    );
  }

  const msBuildTools = MSBuildTools.findAvailableVersion(buildArch, verbose);
  await msBuildTools.buildProject(
    slnFile,
    buildType,
    buildArch,
    msBuildProps,
    verbose,
  );
}

async function nugetRestore(nugetPath, slnFile, verbose, msbuildVersion) {
  const text = 'Restoring NuGet packages ';
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
  const nugetPath = path.join(os.tmpdir(), 'nuget.5.8.0.exe');

  if (!(await existsAsync(nugetPath))) {
    const spinner = newSpinner('Downloading NuGet Binary');
    await downloadFileWithRetry(
      'https://dist.nuget.org/win-x86-commandline/v5.8.0/nuget.exe',
      nugetPath,
      1 /*retries*/,
    );
    spinner.succeed();
  }

  const msbuildTools = MSBuildTools.findAvailableVersion('x86', verbose);
  await nugetRestore(
    nugetPath,
    slnFile,
    verbose,
    msbuildTools.installationVersion,
  );
}

function getSolutionFile(options) {
  const solutions = glob.sync(path.join(options.root, 'windows/*.sln'));
  if (solutions.length == 0) {
    return null;
  } else if (solutions.length == 1) {
    return solutions[0];
  } else {
    console.log(chalk.red('More than one solution file found:'));
    console.log(chalk.bold(solutions.map(x => fs.realpathSync(x)).join('\n')));
    console.log('Use --sln {slnFile} to specify which one to build');
    return null;
  }
}

function parseMsBuildProps(options) {
  let result = {};
  if (options.msbuildprops) {
    const props = options.msbuildprops.split(',');
    for (let i = 0; i < props.length; i++) {
      const prop = props[i].split('=');
      result[prop[0]] = prop[1];
    }
  }
  return result;
}

async function downloadFileWithRetry(url, dest, retries) {
  for (let retryCount = 0; ; ++retryCount) {
    try {
      return await downloadFile(url, dest);
    } catch (ex) {
      if (retryCount === retries) {
        throw ex;
      }
    }
  }
}

function downloadFile(url, dest) {
  const destFile = fs.createWriteStream(dest);

  return new Promise((resolve, reject) => {
    https
      .get(url)
      .on('response', res => res.pipe(destFile))
      .on('finish', () => {
        destFile.on('finish', () => {
          destFile.close();
          resolve();
        });
      })
      .on('error', err => {
        fs.unlink(dest, () => reject(err));
      });
  });
}

module.exports = {
  buildSolution,
  getSolutionFile,
  restoreNuGetPackages,
  parseMsBuildProps,
};
