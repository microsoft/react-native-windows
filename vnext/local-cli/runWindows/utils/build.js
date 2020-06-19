/**
 * Copyright (c) Microsoft Corporation.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check
'use strict';

const fs = require('fs');
const os = require('os');
const path = require('path');
const {execSync} = require('child_process');

const MSBuildTools = require('./msbuildtools');
const Version = require('./version');
const {
  commandWithProgress,
  newSpinner,
  newError,
} = require('./commandWithProgress');
const util = require('util');

const existsAsync = util.promisify(fs.exists);

async function buildSolution(
  slnFile,
  buildType,
  buildArch,
  msBuildProps,
  verbose,
  target,
  buildLogDirectory,
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
    target,
    buildLogDirectory,
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
  let nugetPath =
    options.nugetPath || path.join(os.tmpdir(), 'nuget.4.9.2.exe');

  let downloadedNuget = false;
  const ensureNugetSpinner = newSpinner('Locating NuGet executable');
  if (!(await existsAsync(nugetPath))) {
    try {
      nugetPath = execSync('where nuget')
        .toString()
        .trim();
    } catch {}
  }

  if (!(await existsAsync(nugetPath))) {
    await commandWithProgress(
      ensureNugetSpinner,
      'Downloading NuGet Binary',
      'powershell',
      `$progressPreference = [System.Management.Automation.ActionPreference]::SilentlyContinue; Invoke-WebRequest https://dist.nuget.org/win-x86-commandline/v4.9.2/nuget.exe -outfile ${nugetPath}`.split(
        ' ',
      ),
      verbose,
    );
    downloadedNuget = true;
  }
  ensureNugetSpinner.succeed('Found NuGet Binary');

  if (verbose) {
    console.log(`Using Nuget: ${nugetPath}`);
  }

  const msbuildTools = MSBuildTools.findAvailableVersion('x86', verbose);
  try {
    await nugetRestore(
      nugetPath,
      slnFile,
      verbose,
      msbuildTools.installationVersion,
    );
  } catch (e) {
    if (!options.isRetryingNuget) {
      const retryOptions = Object.assign({isRetryingNuget: true}, options);
      if (downloadedNuget) {
        fs.unlinkSync(nugetPath);
      }
      return restoreNuGetPackages(retryOptions, slnFile, verbose);
    }
    throw e;
  }
}

const configErrorString = 'Error: ';

function getAppSolutionFile(options, config) {
  // Use the solution file if specified
  if (options.sln) {
    return path.join(options.root, options.sln);
  }

  // Check the answer from react-native config
  const windowsAppConfig = config.project.windows;
  const configSolutionFile = windowsAppConfig.solutionFile;

  if (configSolutionFile.startsWith(configErrorString)) {
    newError(
      configSolutionFile.substr(configErrorString.length) +
        ' Optionally, use --sln {slnFile}.',
    );
    return null;
  } else {
    return path.join(
      windowsAppConfig.folder,
      windowsAppConfig.sourceDir,
      configSolutionFile,
    );
  }
}

function getAppProjectFile(options, config) {
  // Use the project file if specified
  if (options.proj) {
    return path.join(options.root, options.proj);
  }

  // Check the answer from react-native config
  const windowsAppConfig = config.project.windows;
  const configProject = windowsAppConfig.project;

  if (
    typeof configProject === 'string' &&
    configProject.startsWith(configErrorString)
  ) {
    newError(
      configProject.substr(configErrorString.length) +
        ' Optionally, use --proj {projFile}.',
    );
    return null;
  } else {
    const configProjectFile = configProject.projectFile;
    if (configProjectFile.startsWith(configErrorString)) {
      newError(
        configProjectFile.substr(configErrorString.length) +
          ' Optionally, use --proj {projFile}.',
      );
      return null;
    }
    return path.join(
      windowsAppConfig.folder,
      windowsAppConfig.sourceDir,
      configProjectFile,
    );
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

module.exports = {
  buildSolution,
  getAppSolutionFile,
  getAppProjectFile,
  restoreNuGetPackages,
  parseMsBuildProps,
};
