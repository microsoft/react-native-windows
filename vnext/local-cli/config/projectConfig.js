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
  folder: string,       // (auto) Absolute path to the app root folder, determined by react-native config, ex: 'c:\path\to\my-app'
  sourceDir: string,    // (req) Relative path to the windows implementation under folder, ex: 'windows'
  solutionFile: string, // (req) Relative path to the app's VS solution file under sourceDir, ex: 'MyApp.sln'
  project: { // (req)
    projectFile: string, // (req) Relative path to the VS project file under sourceDir, ex: 'MyApp\MyApp.vcxproj' for 'c:\path\to\my-app\windows\MyApp\MyApp.vcxproj'
    projectName: string, // (auto) Name of the project, determined from projectFile, ex: 'MyApp'
    projectLang: string, // (auto) Language of the project, cpp or cs, determined from projectFile
    projectGuid: string, // (auto) Project identifier, determined from projectFile
  },
}

Example react-native.config.js:

module.exports = {
  project: {
    windows: {
      sourceDir: 'windows',
      solutionFile: 'MyApp.sln',
      project: {
        projectFile: 'MyApp\\MyApp.vcxproj',
      },
    },
  },
};

*/

/**
 * Gets the config of any RNW apps under the target folder.
 * @param {string} folder The absolute path to the target folder.
 * @param {object} userConfig A manually specified override config.
 * @return {object} The config if any RNW apps exist.
 */
function projectConfigWindows(folder, userConfig = {}) {
  if (userConfig === null) {
    return null;
  }

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
        throw new Error(
          `${item} is required but not specified in react-native.config`,
        );
      }
    });
  }

  const usingManualSolutionFile = 'solutionFile' in userConfig;

  var solutionFile = null;
  if (usingManualSolutionFile && userConfig.solutionFile !== null) {
    // Manually provided solutionFile, so extract it
    solutionFile = path.join(sourceDir, userConfig.solutionFile);
  } else if (!usingManualSolutionFile) {
    // No manually provided solutionFile, try to find it
    const foundSolutions = configUtils.findSolutionFiles(sourceDir);
    if (foundSolutions.length === 0) {
      throw new Error(
        'No app solution file found, please specify in react-native.config',
      );
    } else if (foundSolutions.length > 1) {
      throw new Error(
        'Too many app solution files found, please specify in react-native.config',
      );
    }
    solutionFile = path.join(sourceDir, foundSolutions[0]);
  }

  if (solutionFile === null) {
    throw new Error(
      'Unable to determine app solution file, please specify in react-native.config',
    );
  }

  const usingManualProjectOverride = 'project' in userConfig;

  var projectFile = null;
  if (usingManualProjectOverride) {
    // Manually provided project, so extract it
    if (!('projectFile' in userConfig.project)) {
      throw new Error(
        'projectFile is required for project in react-native.config',
      );
    }
    projectFile = path.join(sourceDir, userConfig.project.projectFile);
  } else {
    // No manually provided project, try to find it
    const foundProjects = configUtils.findAppProjectFiles(sourceDir);
    if (foundProjects.length === 0) {
      throw new Error(
        'No app project file found, please specify in react-native.config',
      );
    } else if (foundProjects.length > 1) {
      throw new Error(
        'Too many app project files found, please specify in react-native.config',
      );
    }
    projectFile = path.join(sourceDir, foundProjects[0]);
  }

  if (projectFile === null) {
    throw new Error(
      'Unable to determine app project file, please specify in react-native.config',
    );
  }

  const projectContents = configUtils.readProjectFile(projectFile);

  return {
    folder,
    sourceDir: sourceDir.substr(folder.length + 1),
    solutionFile: solutionFile.substr(sourceDir.length + 1),
    project: {
      projectFile: projectFile.substr(sourceDir.length + 1),
      projectName: configUtils.getProjectName(projectContents),
      projectLang: configUtils.getProjectLanguage(projectFile),
      projectGuid: configUtils.getProjectGuid(projectContents),
    },
  };
}

module.exports = {
  projectConfigWindows: projectConfigWindows,
};
