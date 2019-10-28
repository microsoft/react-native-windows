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

    args.push('/p:PlatformToolset=v141');

    // Set platform toolset for VS 2017 (this way we can keep the base sln file working for VS 2015)
    if (this.version === '15.0') {
      args.push('/p:VisualStudioVersion=15.0');
    } else if (this.version === '16.0') {
      args.push('/p:VisualStudioVersion=16.0');
    }
    args.push('/bl');

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
    process.env['ProgramFiles(x86)'] || process.env.ProgramFiles,
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
        let toolsPathOutputStr = toolsPathOutput[1];
        // Win10 on .NET Native uses x86 arch compiler, if using x64 Node, use x86 tools
        if (
          version === '15.0' ||
          (version === '14.0' && toolsPathOutputStr.indexOf('amd64') > -1)
        ) {
          toolsPathOutputStr = path.resolve(toolsPathOutputStr, '..');
        }
        toolsPath = toolsPathOutputStr;
      }
    } catch (e) {
      toolsPath = null;
    }
    return toolsPath;
  }
}

function getVC141Component(version, buildArch) {
  if (version === '16.0') {
    switch (buildArch.toLowerCase()) {
      case 'x86':
      case 'x64':
        return 'Microsoft.VisualStudio.Component.VC.v141.x86.x64';
      case 'arm':
        return 'Microsoft.VisualStudio.Component.VC.v141.ARM';
      case 'arm64':
        return 'Microsoft.VisualStudio.Component.VC.v141.ARM64';
    }
  } else {
    switch (buildArch.toLowerCase()) {
      case 'x86':
      case 'x64':
        return 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64';
      case 'arm':
        return 'Microsoft.VisualStudio.Component.VC.Tools.ARM';
      case 'arm64':
        return 'Microsoft.VisualStudio.Component.VC.Tools.ARM64';
    }
  }
}

function checkMSBuildVersion(version, buildArch, verbose) {
  let toolsPath = null;
  if (verbose) {
    console.log('Searching for MSBuild version ' + version);
  }

  // https://aka.ms/vs/workloads
  const requires16 = [
    'Microsoft.Component.MSBuild',
    getVC141Component(version, buildArch),
    'Microsoft.VisualStudio.ComponentGroup.UWP.VC.v141',
  ];
  const requires15 = [
    'Microsoft.Component.MSBuild',
    getVC141Component(version, buildArch),
    'Microsoft.VisualStudio.ComponentGroup.UWP.VC',
  ];

  const requires = version === '16.0' ? requires16 : requires15;

  const vsPath = VSWhere(requires.join(' '), version, 'installationPath');
  const installationVersion = VSWhere(
    requires.join(' '),
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

module.exports.findAvailableVersion = function(buildArch, verbose) {
  const versions =
    process.env.VisualStudioVersion != null
      ? [
          checkMSBuildVersion(
            process.env.VisualStudioVersion,
            buildArch,
            verbose,
          ),
        ]
      : MSBUILD_VERSIONS.map(function(value) {
          return checkMSBuildVersion(value, buildArch, verbose);
        });
  const msbuildTools = versions.find(Boolean);

  if (!msbuildTools) {
    if (process.env.VisualStudioVersion != null) {
      throw new Error(
        `MSBuild tools not found for version ${
          process.env.VisualStudioVersion
        } (from environment). Make sure all required components have been installed (e.g. v141 support)`,
      );
    } else {
      throw new Error(
        'MSBuild tools not found. Make sure all required components have been installed (e.g. v141 support)',
      );
    }
  }
  return msbuildTools;
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
