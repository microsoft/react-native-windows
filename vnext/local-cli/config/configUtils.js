/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const fs = require('fs');
const path = require('path');
const glob = require('glob');

function findFiles(folder, filenamePattern) {
  const files = glob.sync(path.join('**', filenamePattern), {
    cwd: folder,
    ignore: [
      'node_modules/**',
      '**/Debug/**',
      '**/Release/**',
      '**/Generated Files/**',
      '**/packages/**',
    ],
  });

  return files;
}

function findWindowsFolder(folder) {
  const winDir = 'windows';
  const joinedDir = path.join(folder, winDir);
  if (fs.existsSync(joinedDir)) {
    return joinedDir;
  }

  return null;
}

function isRnwSolution(filePath) {
  return (
    fs
      .readFileSync(filePath)
      .toString()
      .search(/ReactNative/) > 0
  );
}

function findSolutionFiles(winFolder) {
  // First search for all potential solution files
  const allSolutions = findFiles(winFolder, '*.sln');

  if (allSolutions.length === 0) {
    // If there're no solution files, return 0
    return [];
  }

  var solutionFiles = [];

  // Try to find any solution file that appears to be a react native solution
  for (var solutionFile of allSolutions) {
    if (isRnwSolution(path.join(winFolder, solutionFile))) {
      solutionFiles.push(solutionFile);
    }
  }

  return solutionFiles;
}

function isRnwDependencyProject(filePath) {
  return (
    fs
      .readFileSync(filePath)
      .toString()
      .search(/Microsoft\.ReactNative\.Uwp\.(Cpp|CSharp)Lib\.targets/) > 0
  );
}

function findDependencyProjectFiles(winFolder) {
  // First, search for all potential project files
  const allCppProj = findFiles(winFolder, '*.vcxproj');
  const allCsProj = findFiles(winFolder, '*.csproj');

  const allProjects = allCppProj.concat(allCsProj);

  if (allProjects.length === 0) {
    // If there're no project files, return 0
    return [];
  }

  var dependencyProjectFiles = [];

  // Try to find any project file that appears to be a dependency project
  for (var projectFile of allProjects) {
    if (isRnwDependencyProject(path.join(winFolder, projectFile))) {
      dependencyProjectFiles.push(projectFile);
    }
  }

  return dependencyProjectFiles;
}

function isRnwAppProject(filePath) {
  return (
    fs
      .readFileSync(filePath)
      .toString()
      .search(/Microsoft\.ReactNative\.Uwp\.(Cpp|CSharp)App\.targets/) > 0
  );
}

function findAppProjectFiles(winFolder) {
  // First, search for all potential project files
  const allCppProj = findFiles(winFolder, '*.vcxproj');
  const allCsProj = findFiles(winFolder, '*.csproj');

  const allProjects = allCppProj.concat(allCsProj);

  if (allProjects.length === 0) {
    // If there're no project files, return 0
    return [];
  }

  var appProjectFiles = [];

  // Try to find any project file that appears to be an app project
  for (var projectFile of allProjects) {
    if (isRnwAppProject(path.join(winFolder, projectFile))) {
      appProjectFiles.push(projectFile);
    }
  }

  return appProjectFiles;
}

function getProjectLanguage(projectPath) {
  if (projectPath.endsWith('.vcxproj')) {
    return 'cpp';
  } else if (projectPath.endsWith('.csproj')) {
    return 'cs';
  }
  return null;
}

function readProjectFile(projectPath) {
  return fs.readFileSync(projectPath, 'utf8').toString();
}

function findTagValue(projectContents, tagName) {
  const regexExpression = `<${tagName}>(.*)</${tagName}>`;
  const regex = new RegExp(regexExpression, 'm');
  const match = projectContents.match(regex);

  return match !== null && match.length === 2 ? match[1] : null;
}

function getProjectName(projectContents) {
  return (
    findTagValue(projectContents, 'ProjectName') ||
    findTagValue(projectContents, 'AssemblyName')
  );
}

function getProjectNamespace(projectContents) {
  return findTagValue(projectContents, 'RootNamespace');
}

function getProjectGuid(projectContents) {
  return findTagValue(projectContents, 'ProjectGuid');
}

module.exports = {
  findFiles: findFiles,
  findWindowsFolder: findWindowsFolder,
  isRnwSolution: isRnwSolution,
  findSolutionFiles: findSolutionFiles,
  isRnwDependencyProject: isRnwDependencyProject,
  findDependencyProjectFiles: findDependencyProjectFiles,
  isRnwAppProject: isRnwAppProject,
  findAppProjectFiles: findAppProjectFiles,
  getProjectLanguage: getProjectLanguage,
  readProjectFile: readProjectFile,
  getProjectName: getProjectName,
  getProjectNamespace: getProjectNamespace,
  getProjectGuid: getProjectGuid,
};
