/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check
'use strict';

const EOL = require('os').EOL;
const fs = require('fs');
const path = require('path');
const child_process = require('child_process');
const chalk = require('chalk');
const shell = require('shelljs');
const Version = require('./version');
const checkRequirements = require('./checkRequirements');
const {
  commandWithProgress,
  newInfo,
  newSpinner,
  newSuccess,
} = require('./commandWithProgress');

const MSBUILD_VERSIONS = ['16.0', '15.0', '14.0', '12.0', '4.0'];

class MSBuildTools {
  constructor(version, localPath) {
    this.version = version;
    this.path = localPath;
  }

  cleanProject(slnFile) {
    const cmd = `"${path.join(
      this.path,
      'msbuild.exe',
    )}" "${slnFile}" t/:Clean`;
    const results = child_process
      .execSync(cmd)
      .toString()
      .split(EOL);
    results.forEach(result => console.log(chalk.white(result)));
  }

  async buildProject(slnFile, buildType, buildArch, config, verbose) {
    newSuccess(`Found Solution: ${slnFile}`);
    newInfo(`Build configuration: ${buildType}`);
    newInfo(`Build platform: ${buildArch}`);

    const verbosityOption = verbose ? 'normal' : 'quiet';
    const args = [
      `/clp:NoSummary;NoItemAndPropertyList;Verbosity=${verbosityOption}`,
      '/nologo',
      `/p:Configuration=${buildType}`,
      `/p:Platform=${buildArch}`,
      '/p:AppxBundle=Never',
    ];

    // Set platform toolset for VS 2017 (this way we can keep the base sln file working for VS 2015)
    if (this.version === '15.0') {
      args.push('/p:PlatformToolset=v141');
      args.push('/p:VisualStudioVersion=15.0');
    }

    // Set platform toolset for VS 2019
    if (this.version === '16.0') {
      args.push('/p:PlatformToolset=v142');
      args.push('/p:VisualStudioVersion=16.0');
    }

    if (config) {
      Object.keys(config).forEach(function(key) {
        args.push(`/p:${key}=${config[key]}`);
      });
    }

    try {
      checkRequirements.isWinSdkPresent('10.0');
    } catch (e) {
      console.log(chalk.red(e.message));
      return;
    }

    const progressName = 'Building Solution';
    const spinner = newSpinner(progressName);
    await commandWithProgress(
      spinner,
      progressName,
      path.join(this.path, 'msbuild.exe'),
      [slnFile].concat(args),
    );
    // Always inherit from stdio as we're controlling verbosity output above.
    // child_process.execSync(cmd, {stdio: 'inherit'});
  }
}

function checkMSBuildVersion(version) {
  let toolsPath = null;

  // This path is maintained and VS has promised to keep it valid.
  const vsWherePath = path.join(
    process.env['ProgramFiles(x86)'],
    '/Microsoft Visual Studio/Installer/vswhere.exe',
  );

  // Check if vswhere is present and try to find MSBuild.
  if (fs.existsSync(vsWherePath)) {
    const vsPath = child_process
      .execSync(
        `"${vsWherePath}" -latest -products * Microsoft.Component.MSBuild -property installationPath`,
      )
      .toString()
      .split(EOL)[0];

    // VS 2019 changed path naming convention
    const vsVersion = version === '16.0' ? 'Current' : version;

    // look for the specified version of msbuild
    const msBuildPath = path.join(
      vsPath,
      'MSBuild',
      vsVersion,
      'Bin/MSBuild.exe',
    );

    toolsPath = fs.existsSync(msBuildPath) ? path.dirname(msBuildPath) : null;
  } else {
    const query = `reg query HKLM\\SOFTWARE\\Microsoft\\MSBuild\\ToolsVersions\\${version} /s /v MSBuildToolsPath`;
    // Try to get the MSBuild path using registry
    try {
      const output = child_process.execSync(query).toString();
      let toolsPathOutput = /MSBuildToolsPath\s+REG_SZ\s+(.*)/i.exec(output);
      if (toolsPathOutput) {
        toolsPathOutput = toolsPathOutput[1];
        // Win10 on .NET Native uses x86 arch compiler, if using x64 Node, use x86 tools
        if (
          version === '15.0' ||
          (version === '14.0' && toolsPath.indexOf('amd64') > -1)
        ) {
          toolsPathOutput = path.resolve(toolsPathOutput, '..');
        }
        toolsPath = toolsPathOutput;
      }
    } catch (e) {
      toolsPath = null;
    }
  }

  // We found something so return MSBuild Tools.
  if (toolsPath) {
    newSuccess(`Found MSBuild v${version} at ${toolsPath}`);
    return new MSBuildTools(version, toolsPath);
  } else {
    return null;
  }
}

module.exports.findAvailableVersion = function() {
  const versions = MSBUILD_VERSIONS.map(checkMSBuildVersion);
  const msbuildTools = versions.find(Boolean);

  if (!msbuildTools) {
    throw new Error('MSBuild tools not found');
  }

  return msbuildTools;
};

module.exports.findAllAvailableVersions = function() {
  console.log(chalk.green('Searching for available MSBuild versions...'));
  return MSBUILD_VERSIONS.map(checkMSBuildVersion).filter(item => !!item);
};

module.exports.getAllAvailableUAPVersions = function() {
  const results = [];

  const programFilesFolder =
    process.env['ProgramFiles(x86)'] || process.env.ProgramFiles;
  // No Program Files folder found, so we won't be able to find UAP SDK
  if (!programFilesFolder) {
    return results;
  }

  const uapFolderPath = path.join(
    programFilesFolder,
    'Windows Kits',
    '10',
    'Platforms',
    'UAP',
  );
  // No UAP SDK exists on this machine
  if (!shell.test('-e', uapFolderPath)) {
    return results;
  }

  shell
    .ls(uapFolderPath)
    .filter(uapDir => shell.test('-d', path.join(uapFolderPath, uapDir)))
    .map(Version.tryParse)
    .forEach(version => version && results.push(version));

  return results;
};
