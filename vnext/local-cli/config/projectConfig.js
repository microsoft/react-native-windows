/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const path = require('path');

const configUtils = require('./configUtils.js');

/*

Schema for app projects:

{
  folder: '', // absolute path to the project root, ex: c:\path\project
  sourceDir: '', // relative path to the windows implementation folder, ex: windows
  solutionFile: '', // relative path to the solution, ex: ProjectName.sln
  project: {
    projectFile: '', // relative path to the project, ex: ProjectName\ProjectName.vcxproj
    projectName: '', // name of the project
    projectLang: '', // language of the project, cpp or cs
    projectGuid: '', // project identifier
  },
}

*/

function projectConfigWindows(folder, userConfig = {}) {
  if (userConfig.sourceDir) {
    return {
      folder,
      sourceDir: userConfig.sourceDir,
      solutionFile: userConfig.solutionFile,
      project: userConfig.project,
    };
  }

  const sourceDir = configUtils.findWindowsFolder(folder);

  if (!sourceDir) {
    return null;
  }

  const packageJson = require(path.join(folder, 'package.json'));

  const solutionFile = configUtils.findSolutionFile(sourceDir, packageJson);

  const projectFile = configUtils.findProjectFile(sourceDir, packageJson);

  const projectLang = configUtils.getProjectLanguage(projectFile);

  const projectXml = configUtils.readProjectFile(projectFile);

  const projectName = configUtils.getProjectName(projectXml);

  const projectGuid = configUtils.getProjectGuid(projectXml);

  return {
    folder,
    sourceDir: sourceDir.substr(folder.length + 1),
    solutionFile: solutionFile.substr(sourceDir.length + 1),
    project: {
      projectFile: projectFile.substr(sourceDir.length + 1),
      projectName,
      projectLang,
      projectGuid,
    },
  };
}

module.exports = {
  projectConfigWindows: projectConfigWindows,
};
