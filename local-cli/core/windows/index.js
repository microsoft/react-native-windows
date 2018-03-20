/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 */
'use strict';

const findWindowsSolution = require('./findWindowsSolution');
const findNamespace = require('./findNamespace');
const findProject = require('./findProject');
const findDependencyProject = require('./findDependencyProject');
const findProjectName = require('./findProjectName');
const findProjectGUID = require('./findProjectGUID');
const findPackageClassName = require('./findPackageClassName');
const findMainFile = require('./findMainFile');
const path = require('path');

const relativeProjectPath = (fullProjPath) => {
  const windowsPath = fullProjPath
                  .substring(fullProjPath.lastIndexOf("node_modules") - 1, fullProjPath.length)
                  .replace(/\//g, '\\');

  return path.join('..', windowsPath);
};

/**
 * Gets windows project config by analyzing given folder and taking some
 * defaults specified by user into consideration
 */
exports.projectConfig = function projectConfigWindows(folder, userConfig) {

  const csSolution = userConfig.csSolution || findWindowsSolution(folder);
  const projectPath = userConfig.projectPath || findProject(folder);

  if (!csSolution || !projectPath) {
    return null;
  }

  // expects solutions to be named the same as project folders
  const solutionPath = path.join(folder, csSolution);
  const sourceDir = userConfig.sourceDir || path.dirname(projectPath);
  const mainFilePath = findMainFile(sourceDir);

  return {
    sourceDir,
    solutionPath,
    projectPath,
    mainFilePath,
    folder,
    userConfig,
  };
};

/**
 * Same as projectConfigWindows except it returns
 * different config that applies to packages only
 */
exports.dependencyConfig = function dependencyConfigWindows(folder, userConfig) {

  const projectPath = userConfig.projectPath || findDependencyProject(folder);

  if (!projectPath) {
    return null;
  }

  const sourceDir = userConfig.sourceDir || path.dirname(projectPath);
  const packageClassName = findPackageClassName(sourceDir);
  const namespace = userConfig.namespace || findNamespace(sourceDir);

  /**
   * This module has no package to export or no namespace
   */
  if (!packageClassName || !namespace) {
    return null;
  }

  const packageUsingPath = userConfig.packageUsingPath ||
    `using ${namespace};`;

  const packageInstance = userConfig.packageInstance ||
    `new ${packageClassName}()`;

  const projectGUID = findProjectGUID(projectPath);
  const projectName = findProjectName(projectPath);
  const relativeProjPath = relativeProjectPath(projectPath);

  return {
    sourceDir,
    packageUsingPath,
    packageInstance,
    projectName,
    projectPath,
    folder,
    projectGUID,
    relativeProjPath,
  };
};
