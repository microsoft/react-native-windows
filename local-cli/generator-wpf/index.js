'use strict';
const fs = require('fs');
const chalk = require('chalk');
const path = require('path');
const copyAndReplace = require('react-native-local-cli/util/copyAndReplace');
const walk = require('react-native-local-cli/util/walk');
const prompt = require('react-native-local-cli/generator/promptSync')();
const uuid = require('uuid');

function createDir(destPath) {
  if (!fs.existsSync(destPath)) {
    fs.mkdirSync(destPath);
  }
}

function copyAndReplaceWithChangedCallback(srcPath, destRoot, relativeDestPath, replacements) {
  if (!replacements) {
    replacements = {};
  }
  const contentChangedCallback = (_, contentChanged) =>
    upgradeFileContentChangedCallback(
      srcPath,
      relativeDestPath,
      contentChanged
    );

  copyAndReplace(
    srcPath,
    path.join(destRoot, relativeDestPath),
    replacements,
    contentChangedCallback
  );
}

function copyAndReplaceAll(srcPath, destPath, relativeDestDir, replacements) {
  walk(srcPath).forEach(absoluteSrcFilePath => {
    const filename = path.relative(srcPath, absoluteSrcFilePath);
    const relativeDestPath = path.join(relativeDestDir, filename);
    copyAndReplaceWithChangedCallback(absoluteSrcFilePath, destPath, relativeDestPath, replacements);
  });
}

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
  const wpfDir = 'wpf';
  const reactAssetsDir = 'ReactAssets';
  const projDir = 'proj';

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

function upgradeFileContentChangedCallback(
  absoluteSrcFilePath,
  relativeDestPath,
  contentChanged
) {
  if (contentChanged === 'new') {
    console.log(`${chalk.bold('new')} ${relativeDestPath}`);
    return 'overwrite';
  }
  if (contentChanged === 'changed') {
    console.log(
      `${chalk.bold(relativeDestPath)} ` +
      `has changed in the new version.\nDo you want to keep your ${relativeDestPath} or replace it with the ` +
      `latest version?\nIf you ever made any changes ` +
      `to this file, you'll probably want to keep it.\n` +
      `You can see the new version here: ${absoluteSrcFilePath}\n` +
      `Do you want to replace ${relativeDestPath}? ` +
      `Answer y to replace, n to keep your version: `
    );
    const answer = prompt();
    if (answer === 'y') {
      console.log(`Replacing ${relativeDestPath}`);
      return 'overwrite';
    }
    console.log(`Keeping your ${relativeDestPath}`);
    return 'keep';
  }
  if (contentChanged === 'identical') {
    return 'keep';
  }
  throw new Error(
    `Unknown file changed state: ${relativeDestPath}, ${contentChanged}`
  );
}

module.exports = {
  copyProjectTemplateAndReplace
};
