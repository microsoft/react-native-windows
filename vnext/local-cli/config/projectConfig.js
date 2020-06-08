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
    // Nothing to look for here, bail
    return null;
  }

  var result = {
    folder: folder,
    sourceDir: sourceDir.substr(folder.length + 1),
  };

  var validProject = false;

  if (usingManualOverride) {
    // Manual override, try to use it for solutionFile
    if (!('solutionFile' in userConfig)) {
      result.solutionFile =
        'Error: Solution file is required but not specified in react-native.config.';
    } else if (userConfig.solutionFile === null) {
      result.solutionFile =
        'Error: Solution file is null in react-native.config.';
    } else {
      result.solutionFile = userConfig.solutionFile;
    }

    // Manual override, try to use it for project
    if (!('project' in userConfig)) {
      result.project =
        'Error: Project is required but not specified in react-native.config.';
    } else if (userConfig.project === null) {
      result.project = 'Error: Project is null in react-native.config.';
    } else {
      if (!('projectFile' in userConfig.project)) {
        result.project = {
          projectFile:
            'Error: Project file is required for project in react-native.config.',
        };
      } else if (userConfig.project.projectFile === null) {
        result.project = {
          projectFile: 'Error: Project file is null in react-native.config.',
        };
      } else {
        result.project = {
          projectFile: userConfig.project.projectFile,
        };
        validProject = true;
      }
    }
  } else {
    // No manually provided solutionFile, try to find it
    const foundSolutions = configUtils.findSolutionFiles(sourceDir);
    if (foundSolutions.length === 0) {
      result.solutionFile =
        'Error: No app solution file found, please specify in react-native.config.';
    } else if (foundSolutions.length > 1) {
      result.solutionFile =
        'Error: Too many app solution files found, please specify in react-native.config.';
    } else {
      result.solutionFile = foundSolutions[0];
    }

    // No manually provided project, try to find it
    const foundProjects = configUtils.findAppProjectFiles(sourceDir);
    if (foundProjects.length === 0) {
      result.project = {
        projectFile:
          'Error: No app project file found, please specify in react-native.config.',
      };
    } else if (foundProjects.length > 1) {
      result.project = {
        projectFile:
          'Error: Too many app project files found, please specify in react-native.config.',
      };
    } else {
      result.project = {
        projectFile: foundProjects[0],
      };
      validProject = true;
    }
  }

  if (validProject) {
    const projectFile = path.join(sourceDir, result.project.projectFile);
    const projectContents = configUtils.readProjectFile(projectFile);

    // Add missing (auto) items
    result.project.projectName = configUtils.getProjectName(projectContents);
    result.project.projectLang = configUtils.getProjectLanguage(projectFile);
    result.project.projectGuid = configUtils.getProjectGuid(projectContents);
  }

  return result;
}

module.exports = {
  projectConfigWindows: projectConfigWindows,
};
