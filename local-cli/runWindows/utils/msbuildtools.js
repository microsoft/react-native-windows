'use strict';

const EOL = require('os').EOL;
const path = require('path');
const child_process = require('child_process');
const chalk = require('chalk');
const shell = require('shelljs');
const Version = require('./version');
const checkRequirements = require('./checkRequirements');

const MSBUILD_VERSIONS = ['15.0', '14.0', '12.0', '4.0'];

class MSBuildTools {
  constructor(version, localPath) {
    this.version = version;
    this.path = localPath;
  }

  cleanProject(slnFile) {
    const cmd = `"${path.join(this.path, 'msbuild.exe')}" "${slnFile}" t/:Clean`;
    const results = child_process.execSync(cmd).toString().split(EOL);
    results.forEach(result => console.log(chalk.white(result)));
  }

  buildProject(slnFile, buildType, buildArch, config) {
    console.log(chalk.green(`Building Solution: ${slnFile}`));
    console.log(chalk.green(`Build configuration: ${buildType}`));
    console.log(chalk.green(`Build platform: ${buildArch}`));

    const args = [
      '/clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal',
      '/nologo',
      `/p:Configuration=${buildType}`,
      `/p:Platform=${buildArch}`
    ];

    if (config) {
      Object.keys(config).forEach(function (key) {
        args.push(`/p:${key}=${config[key]}`);
      });
    }

    try {
      checkRequirements.isWinSdkPresent('10.0');
    } catch (e) {
      console.log(chalk.red(e.message));
      return;
    }

    const cmd = `"${path.join(this.path, 'msbuild.exe')}" ` + [slnFile].concat(args).join(' ');
    const results = child_process.execSync(cmd).toString().split(EOL);
    results.forEach(result => console.log(chalk.white(result)));
  }
}

function checkMSBuildVersion(version) {
  const query = `reg query HKLM\\SOFTWARE\\Microsoft\\MSBuild\\ToolsVersions\\${version} /s /v MSBuildToolsPath`;
  try {
    const output = child_process.execSync(query).toString();
    let toolsPath = /MSBuildToolsPath\s+REG_SZ\s+(.*)/i.exec(output);
    if (toolsPath) {
      toolsPath = toolsPath[1];
      // Win10 on .NET Native uses x86 arch compiler, if using x64 Node, use x86 tools
      if ((version === '15.0' || version === '14.0' && toolsPath.indexOf('amd64') > -1)) {
        toolsPath = path.resolve(toolsPath, '..');
      }
      console.log(chalk.green(`Found MSBuild v${version} at ${toolsPath}`));
      return new MSBuildTools(version, toolsPath);
    }
  } catch (e) {
    return null;
  }
}

module.exports.findAvailableVersion = function () {
  const versions = MSBUILD_VERSIONS.map(checkMSBuildVersion);
  const msbuildTools = versions[0] || versions[1] || versions[2] || versions[3];
  if (!msbuildTools) {
    throw new Error('MSBuild tools not found');
  }

  return msbuildTools;
};

module.exports.findAllAvailableVersions = function () {
  console.log(chalk.green('Searching for available MSBuild versions...'));
  return MSBUILD_VERSIONS
    .map(checkMSBuildVersion)
    .filter(item => !!item);
};

module.exports.getAllAvailableUAPVersions = function () {
  const results = [];

  const programFilesFolder = process.env['ProgramFiles(x86)'] || process.env.ProgramFiles;
  // No Program Files folder found, so we won't be able to find UAP SDK
  if (!programFilesFolder) {
    return results;
  }

  const uapFolderPath = path.join(programFilesFolder, 'Windows Kits', '10', 'Platforms', 'UAP');
  // No UAP SDK exists on this machine
  if (!shell.test('-e', uapFolderPath)) {
    return results;
  }

  shell.ls(uapFolderPath)
    .filter(uapDir => shell.test('-d', path.join(uapFolderPath, uapDir)))
    .map(Version.tryParse)
    .forEach(version => version && results.push(version));

  return results;
};
