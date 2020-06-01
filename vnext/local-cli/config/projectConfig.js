/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const path = require('path');

const configUtils = require('./configUtils.js');

/*

react-native config will generate the following JSON for app projects that have a
windows implementation, as a target for auto-linking. This is done heurestically,
so if the result isn't quite correct, app developers can provide a manual override
file: react-native.config.

Schema for app projects:

Tags:
auto - Item is always calculated by config. An override file should NEVER provide it.
req  - Item is required. If an override file exists, it MUST provide it. If no override file exists, config will try to calculate it.
opt  - Item is optional. If an override file exists, it MAY provide it. If no override file exists, config may try to calculate it.

{
  folder: string,       // (auto) Absolute path to the app root folder, determined by react-native config, ex: 'c:\path\to\app-name'
  sourceDir: string,    // (req) Relative path to the windows implementation under folder, ex: 'windows'
  solutionFile: string, // (req) Relative path to the app's VS solution file under sourceDir, ex: 'AppName.sln'
  project: { // (req)
    projectFile: string, // (req) Relative path to the VS project file under sourceDir, ex: 'AppName\AppName.vcxproj' for 'c:\path\to\app-name\windows\AppName\AppName.vcxproj'
    projectName: string, // (auto) Name of the project, determined from projectFile, ex: 'AppName'
    projectLang: string, // (auto) Language of the project, cpp or cs, determined from projectFile
    projectGuid: string, // (auto) Project identifier, determined from projectFile
  },
}

*/

function projectConfigWindows(folder, userConfig = {}) {
  const usingManualOverride = 'sourceDir' in userConfig;

  const sourceDir = usingManualOverride
    ? path.join(folder, userConfig.sourceDir)
    : configUtils.findWindowsFolder(folder);

  if (sourceDir === null) {
    return null;
  }

  const alwaysRequired = ['solutionFile', 'project'];

  if (usingManualOverride) {
    // Verifying (req) items
    alwaysRequired.forEach(item => {
      if (!(item in userConfig)) {
        throw `${item} is required but not specified in react-native.config`;
      }
    });
  }

  const packageJson = require(path.join(folder, 'package.json'));

  const solutionFile = usingManualOverride
    ? path.join(sourceDir, userConfig.solutionFile)
    : configUtils.findSolutionFile(sourceDir, packageJson);

  if (usingManualOverride && !('projectFile' in userConfig.project)) {
    throw 'projectFile is required but not specified in react-native.config';
  }

  const projectFile = usingManualOverride
    ? path.join(sourceDir, userConfig.project.projectFile)
    : configUtils.findProjectFile(sourceDir, packageJson);

  const projectXml = configUtils.readProjectFile(projectFile);

  return {
    folder,
    sourceDir: sourceDir.substr(folder.length + 1),
    solutionFile: solutionFile.substr(sourceDir.length + 1),
    project: {
      projectFile: projectFile.substr(sourceDir.length + 1),
      projectName: configUtils.getProjectName(projectXml),
      projectLang: configUtils.getProjectLanguage(projectFile),
      projectGuid: configUtils.getProjectGuid(projectXml),
    },
  };
}

module.exports = {
  projectConfigWindows: projectConfigWindows,
};
