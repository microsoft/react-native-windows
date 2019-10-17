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
  newError,
} = require('./commandWithProgress');

const MSBUILD_VERSIONS = ['16.0', '15.0', '14.0', '12.0', '4.0'];

class MSBuildTools {
  // version is something like 16.0 for 2019
  // localPath is the path to MSBuild.exe (x86)
  // installationVersion is the full version e.g. 16.3.29411.108
  constructor(version, localPath, installationVersion) {
    this.version = version;
    this.path = localPath;
    this.installationVersion = installationVersion;
  }

  cleanProject(slnFile) {
    const cmd = `"${path.join(
      this.path,
      'msbuild.exe',
    )}" "${slnFile}" /t:Clean`;
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

    //const verbosityOption = verbose ? 'normal' : 'quiet';
    const verbosityOption = 'normal';
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
      // we are guaranteed to have Microsoft.VisualStudio.ComponentGroup.UWP.VC.v141
      const vsPath = path.resolve(this.path, '../../..');
      const targetsPath = path.join(vsPath, 'MSBuild\\Microsoft\\VC\\v150\\');
      args.push('/p:PlatformToolset=v141');
      args.push('/p:VisualStudioVersion=16.0');
      args.push('/p:VCTargetsPath=' + targetsPath);
    }

    if (config) {
      Object.keys(config).forEach(function(key) {
        args.push(`/p:${key}=${config[key]}`);
      });
    }

    try {
      checkRequirements.isWinSdkPresent('10.0');
    } catch (e) {
      newError(e.message);
      return;
    }

    console.log(`Running MSBuild with args ${args.join(' ')}`);

    const progressName = 'Building Solution';
    const spinner = newSpinner(progressName);
    await commandWithProgress(
      spinner,
      progressName,
      path.join(this.path, 'msbuild.exe'),
      [slnFile].concat(args),
      verbose,
    );
  }
}

function VSWhere(requires, version, property) {
  // This path is maintained and VS has promised to keep it valid.
  const vsWherePath = path.join(
    process.env['ProgramFiles(x86)'],
    '/Microsoft Visual Studio/Installer/vswhere.exe',
  );

  // Check if vswhere is present and try to find MSBuild.
  if (fs.existsSync(vsWherePath)) {
    const vsPath = child_process
      .execSync(
        `"${vsWherePath}" -version [${version},${Number(version) +
          1}) -products * -requires ${requires} -property ${property}`,
      )
      .toString()
      .split(EOL)[0];
    return vsPath;
  } else {
    const query = `reg query HKLM\\SOFTWARE\\Microsoft\\MSBuild\\ToolsVersions\\${version} /s /v MSBuildToolsPath`;
    let toolsPath = null;
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
    return toolsPath;
  }
}

function checkMSBuildVersion(version) {
  let toolsPath = null;
  if (process.argv.indexOf('--logging') >= 0) {
    console.log('Searching for MSBuild version ' + version);
  }

  const requiresUWP =
    'Microsoft.Component.MSBuild Microsoft.VisualStudio.ComponentGroup.UWP.VC.v141';
  const vsPath = VSWhere(requiresUWP, version, 'installationPath');
  const installationVersion = VSWhere(
    requiresUWP,
    version,
    'installationVersion',
  );
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

  // We found something so return MSBuild Tools.
  if (toolsPath) {
    newSuccess(
      `Found MSBuild v${version} at ${toolsPath} (${installationVersion})`,
    );
    return new MSBuildTools(version, toolsPath, installationVersion);
  } else {
    return null;
  }
}

module.exports.findAvailableVersion = function() {
  const versions =
    process.env.VisualStudioVersion != null
      ? [checkMSBuildVersion(process.env.VisualStudioVersion)]
      : MSBUILD_VERSIONS.map(checkMSBuildVersion);
  const msbuildTools = versions.find(Boolean);

  if (!msbuildTools) {
    if (process.env.VisualStudioVersion != null) {
      throw new Error(
        `MSBuild tools not found for version ${
          process.env.VisualStudioVersion
        } (from environment). Make sure all components have been installed (e.g. UWP v141 support)`,
      );
    } else {
      throw new Error(
        'MSBuild tools not found. Make sure all components have been installed (e.g. UWP v141 support)',
      );
    }
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
