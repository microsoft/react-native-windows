/**
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 * @format
 */
// @ts-check

const fs = require('fs');
const path = require('path');
const glob = require('glob');

/**
 * Search for files matching the pattern under the target folder.
 * @param {string} folder The absolute path to target folder.
 * @param {string} filenamePattern The pattern to search for.
 * @return {array} Return the array of relative file paths.
 */
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

/**
 * Search for the windows sub-folder under the target folder.
 * @param {string} folder The absolute path to the target folder.
 * @return The absolute path to the windows folder, if it exists.
 */
function findWindowsFolder(folder) {
  const winDir = 'windows';
  const joinedDir = path.join(folder, winDir);
  if (fs.existsSync(joinedDir)) {
    return joinedDir;
  }

  return null;
}

/**
 * Checks if the target file path is a RNW solution file.
 * @param {string} filePath The absolute file path to check.
 * @return {boolean} Whether the path is to a RNW solution file.
 */
function isRnwSolution(filePath) {
  return (
    fs
      .readFileSync(filePath)
      .toString()
      .search(/ReactNative/) > 0
  );
}

/**
 * Search for the RNW solution files under the target folder.
 * @param {string} winFolder The absolute path to target folder.
 * @return {array} Return the array of relative file paths.
 */
function findSolutionFiles(winFolder) {
  // First search for all potential solution files
  const allSolutions = findFiles(winFolder, '*.sln');

  if (allSolutions.length === 0) {
    // If there're no solution files, return 0
    return [];
  } else if (allSolutions.length === 1) {
    // If there is exactly one solution file, assume it's it
    return [allSolutions[0]];
  }

  var solutionFiles = [];

  // Try to find any solution file that appears to be a react native solution
  for (const solutionFile of allSolutions) {
    if (isRnwSolution(path.join(winFolder, solutionFile))) {
      solutionFiles.push(solutionFile);
    }
  }

  return solutionFiles;
}

/**
 * Checks if the target file path is a RNW lib project file.
 * @param {string} filePath The absolute file path to check.
 * @return {boolean} Whether the path is to a RNW lib project file.
 */
function isRnwDependencyProject(filePath) {
  return (
    fs
      .readFileSync(filePath)
      .toString()
      .search(/Microsoft\.ReactNative\.Uwp\.(Cpp|CSharp)Lib\.targets/) > 0
  );
}

/**
 * Search for the RNW lib project files under the target folder.
 * @param {string} winFolder The absolute path to target folder.
 * @return {array} Return the array of relative file paths.
 */
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
  for (const projectFile of allProjects) {
    if (isRnwDependencyProject(path.join(winFolder, projectFile))) {
      dependencyProjectFiles.push(projectFile);
    }
  }

  return dependencyProjectFiles;
}

/**
 * Checks if the target file path is a RNW app project file.
 * @param {string} filePath The absolute file path to check.
 * @return {boolean} Whether the path is to a RNW app project file.
 */
function isRnwAppProject(filePath) {
  return (
    fs
      .readFileSync(filePath)
      .toString()
      .search(/Microsoft\.ReactNative\.Uwp\.(Cpp|CSharp)App\.targets/) > 0
  );
}

/**
 * Search for the RNW app project files under the target folder.
 * @param {string} winFolder The absolute path to target folder.
 * @return {array} Return the array of relative file paths.
 */
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
  for (const projectFile of allProjects) {
    if (isRnwAppProject(path.join(winFolder, projectFile))) {
      appProjectFiles.push(projectFile);
    }
  }

  return appProjectFiles;
}

/**
 * Returns the programming language of the project file.
 * @param {string} projectPath The project file path to check.
 * @return {string} The language string: cpp, cs, or null if unknown.
 */
function getProjectLanguage(projectPath) {
  if (projectPath.endsWith('.vcxproj')) {
    return 'cpp';
  } else if (projectPath.endsWith('.csproj')) {
    return 'cs';
  }
  return null;
}

/**
 * Reads in the contents of the target project file.
 * @param {string} projectPath The target project file path.
 * @return {string} The project file contents.
 */
function readProjectFile(projectPath) {
  return fs.readFileSync(projectPath, 'utf8').toString();
}

/**
 * Search for the given XML tag in the project contents and return its value.
 * @param {string} projectContents The XML project contents.
 * @param {string} tagName The XML tag to look for.
 * @return {string} The value of the tag if it exists.
 */
function findTagValue(projectContents, tagName) {
  const regexExpression = `<${tagName}>(.*)</${tagName}>`;
  const regex = new RegExp(regexExpression, 'm');
  const match = projectContents.match(regex);

  return match !== null && match.length === 2 ? match[1] : null;
}

/**
 * Gets the name of the project from the project contents.
 * @param {string} projectContents The XML project contents.
 * @return {string} The project name.
 */
function getProjectName(projectContents) {
  return (
    findTagValue(projectContents, 'ProjectName') ||
    findTagValue(projectContents, 'AssemblyName') ||
    ''
  );
}

/**
 * Gets the namespace of the project from the project contents.
 * @param {string} projectContents The XML project contents.
 * @return {string} The project namespace.
 */
function getProjectNamespace(projectContents) {
  return findTagValue(projectContents, 'RootNamespace');
}

/**
 * Gets the guid of the project from the project contents.
 * @param {string} projectContents The XML project contents.
 * @return {string} The project guid.
 */
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
