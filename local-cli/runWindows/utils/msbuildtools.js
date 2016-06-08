'use strict';

var path = require('path');
var child_process = require('child_process');
var chalk = require('chalk');
var shell = require('shelljs');

var MSBUILD_VERSIONS = ['15.0', '14.0', '12.0', '4.0'];

function checkWinSDK(targetPlatform) {
  // TODO: Check for WinSDK
}

function checkMSBuildVersion(version) {
  var query = 'reg query HKLM\\SOFTWARE\\Microsoft\\MSBuild\\ToolsVersions\\' + version + ' /v MSBuildToolsPath';
  try {
    var output = child_process.execSync(query).toString();
    var toolsPath = /MSBuildToolsPath\s+REG_SZ\s+(.*)/i.exec(output);
    if (toolsPath) {
      toolsPath = toolsPath[1];
      // Win10 on .NET Native uses x86 arch compiler, if using x64 Node, use x86 tools
      if ((version === '15.0' || version === '14.0' && toolsPath.indexOf('amd64') > -1)) {
        toolsPath = path.resolve(toolsPath, '..');
      }
      console.log(chalk.green('Found MSBuild v' + version + ' at ' + toolsPath));
      return new MSBuildTools(version, toolsPath);
    }
  } catch (e) {
    return null;
  }
}

module.exports = findAvailableVersion = function () {
  var versions = MSBUILD_VERSIONS.map(checkMSBuildVersion);
  var msbuildTools = versions[0] || versions[1] || versions[2] || versions[3];
  if (!msbuildTools) {
    throw new Error('MSBuild tools not found');
  }
};

var VERSION_EXPRESSION = /^\d{1,8}(\.\d{1,8}){0,3}$/;

function hasVersion(str) {
  if (VERSION_EXPRESSION.test(str)) {
    var parts = str.split('.');
    return parts.length === 4;
  }

  return false;
}

module.exports = getAllAvailableUAPVersions = function () {
  var programFilesFolder = process.env['ProgramFiles(x86)'] || process.env['ProgramFiles'];
  // No Program Files folder found, so we won't be able to find UAP SDK
  if (!programFilesFolder) { 
    return []; 
  }

  var uapFolderPath = path.join(programFilesFolder, 'Windows Kits', '10', 'Platforms', 'UAP');
  if (!shell.test('-e', uapFolderPath)) {
    return []; // No UAP SDK exists on this machine
  }

  return shell.ls(uapFolderPath)
    .filter(function(uapDir) {
      return shell.test('-d', path.join(uapFolderPath, uapDir));
    })
    .filter(function(folder) {
      return hasVersion(folder);
    });
};

function MSBuildTools(version, path) {
  this._version = version;
  this._path = path;
}

MSBuildTools.prototype.buildProject = function (slnFile, buildType, buildArch, config) {
  console.log(chalk.green('Building Solution: ' + slnFile));
  console.log(chalk.green('Build configuration: ' + buildType));
  console.log(chalk.green('Build platform: ' + buildArch));

  var args = [
    '/clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal', 
    '/nologo',
    '/p:Configuration=' + buildType,
    '/p:Platform=' + buildArch
  ];

  if (config) {
    Object.keys(config).forEach(function (key) {
      args.push('/p:' + key + '=' + config[key]);
    });
  }

  // TODO: Check Win10SDK
  var cmd = path.join(this._path, 'msbuild') + [slnFile].concat(args).join(' ');
  var results = child_process.execSync(cmd).toString().split('\r\n');
  results.forEach(function (result) {
    console.log(result);
  });
};


