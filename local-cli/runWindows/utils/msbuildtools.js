'use strict';

var path = require('path');
var child_process = require('child_process');
var chalk = require('chalk');

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
};


