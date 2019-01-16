'use strict';
const chalk = require('chalk');
const path = require('path');
const uuid = require('uuid');
const {
  createDir,
  copyAndReplaceAll,
  copyAndReplaceWithChangedCallback
} = require('../generator-common');

const wpfDir = 'wpf';
const reactAssetsDir = 'ReactAssets';
const projDir = 'proj';

function copyProjectTemplateAndReplace(
  srcPath,
  destPath,
  newProjectName,
  options = {}
) {
  if (!srcPath) {
    throw new Error('Need a path to copy from');
  }
  if (!destPath) {
    throw new Error('Need a path to copy to');
  }
  if (!newProjectName) {
    throw new Error('Need a project name');
  }

  const ns = options.ns || newProjectName;
  const projectGuid = uuid.v4();
  const packageGuid = uuid.v4();

  const templateVars = {
    '<%=ns%>': ns,
    '<%=name%>': newProjectName,
    '<%=projectGuid%>': projectGuid,
    '<%=packageGuid%>': packageGuid
  };

  createDir(path.join(destPath, wpfDir));
  createDir(path.join(destPath, wpfDir, newProjectName));
  createDir(path.join(destPath, wpfDir, newProjectName, reactAssetsDir));

  [
    { from: path.join(srcPath, 'App.windows.js'), to: 'App.windows.js' },
    { from: path.join(srcPath, projDir, 'MyApp.sln'), to: path.join(wpfDir, newProjectName + '.sln') },
    { from: path.join(srcPath, projDir, 'MyApp.csproj'), to: path.join(wpfDir, newProjectName, newProjectName + '.csproj') },
    { from: path.join(srcPath, projDir, 'packages.config'), to: path.join(wpfDir, newProjectName, 'packages.config') },
    { from: path.join(srcPath, projDir, 'App.config'), to: path.join(wpfDir, newProjectName, 'App.config') },
    { from: path.join(srcPath, projDir, 'NuGet.Config'), to: path.join(wpfDir, 'NuGet.Config') },
    { from: path.join(srcPath, '_gitignore'), to: path.join(wpfDir, '.gitignore') },
    { from: path.join(srcPath, 'index.wpf.bundle'), to: path.join(wpfDir, newProjectName, reactAssetsDir, 'index.wpf.bundle') },
  ].forEach((mapping) => copyAndReplaceWithChangedCallback(mapping.from, destPath, mapping.to, templateVars));

  copyAndReplaceAll(path.join(srcPath, 'assets'), destPath, path.join(wpfDir, newProjectName, 'Assets'), templateVars);
  copyAndReplaceAll(path.join(srcPath, 'src'), destPath, path.join(wpfDir, newProjectName), templateVars);

  console.log(chalk.white.bold('To run your app on WPF:'));
  console.log(chalk.white('   react-native run-wpf'));
}

module.exports = {
  copyProjectTemplateAndReplace
};
